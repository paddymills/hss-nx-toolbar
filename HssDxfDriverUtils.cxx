
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"

#include <string>
#include <map>
#include <experimental/filesystem>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/Features_Feature.hxx>

using namespace NXOpen;
using namespace std;

namespace fs = experimental::filesystem;

// strings to be found in sketch names
const map<string, int> WHITELISTED_SKETCHES {
        { "ZINC",       2 },
        { "DOR",        3 },
        { "NOCUT",      3 },
        { "NO_CUT",     3 }
    };

// list of strings to be found in body names
//  or names of features that construct body
const vector<char*> BLACKLISTED_BODIES = { "SHIM" };
const vector<char*> JOB_PROP_KEYS = { "JobNo", "JOB_NUMBER" };
const vector<char*> MARK_PROP_KEYS = { "Mark", "PIECE_MARK" };

struct BodyMinMax {
    string JournalIdentifier;
    double min, max;
    bool is_parent;
    string name;
};

string get_export_name(Body *body)
{
    Part* part = dynamic_cast<Part*>( body->OwningPart() );
    map<string, string> body_names;
    int body_index;

    // TODO: determine body type (web, flange?, other?)
    int number_of_bodies = get_number_of_body_exports(part);

    // get filename (no directories or extensions)
    fs::path *part_path = new fs::path(part->FullPath().GetText());
    string part_filename(part_path->filename().stem().string());

    // build base part file name
    string part_name;
    part_name.append(get_part_property( part, JOB_PROP_KEYS ));
    part_name.append("_");
    part_name.append(get_part_property( part, MARK_PROP_KEYS ));

    // make sure part filename starts with build part_name
    // or part_name is more than just job_
    // could be stale data
    if ( startswith(part_name, part_filename) || part_name.length() < 10 )
        part_name = part_filename;

    /* 
        ****************************************************************
        *                       single body part                       *
        ****************************************************************
    */
    if ( number_of_bodies == 1 )
        return part_name;

    /* 
        ****************************************************************
        *                          named body                          *
        ****************************************************************
    */
    if ( strlen(body->Name().GetText()) > 0 )
        return part_name + "-" + body->Name().GetText();

    /* 
        ****************************************************************
        *            attempt to infer from body boundaries             *
        ****************************************************************
    */
    if ( body_names.empty() )
    {
        if ( part_filename.find("web") != string::npos )
            body_names = get_web_names(part);
    }

    try
    {
        if ( body_names.find( body->JournalIdentifier().GetText() ) != body_names.end() )
            return part_name + "-" + body_names[ body->JournalIdentifier().GetText() ];
    }
    catch (exception &e) {}

    /* 
        ****************************************************************
        *                 default: return as body index                *
        ****************************************************************
    */
    // cannot infer so return "_{index}" (i.e. "_1")
    return part_filename + "_" + to_string(body_index++);
}

map<string, string> get_web_names(Part *part)
{
    map<string, string> result;

    vector<BodyMinMax> bodies;
    BodyBoundary *bound;
    BodyMinMax *bmm;

    for (Body *body: *(part->Bodies()))
    {
        bound = new BodyBoundary(body);

        bmm = new BodyMinMax();
        bmm->JournalIdentifier = body->JournalIdentifier().GetText();
        bmm->min = bound->minimum('x');
        bmm->max = bound->maximum('x');
        bmm->is_parent = false;
        bmm->name = "";

        bodies.push_back(*bmm);
    }

    // get part min/max
    double min_x = bodies[0].min;
    double max_x = bodies[0].max;
    for (auto x: bodies)
    {
        min_x = min(min_x, x.min);
        max_x = max(max_x, x.max);
    }

    // name parent body
    for (auto it = bodies.begin(); it != bodies.end(); it++)
    {
        if (it->min == min_x && it->max == max_x)
        {
            it->is_parent = true;

            for (int i=1; i<bodies.size(); i++)
                it->name.append("W" + to_string(i));

            break;
        }
    }
    
    // name child bodies
    for (auto it = bodies.begin(); it != bodies.end(); it++)
    {
        if (it->is_parent)
            continue;

        int index = 1;
        for (auto other: bodies)
        {
            if (other.is_parent);
            else if (other.JournalIdentifier == it->JournalIdentifier);
            else if (other.min < it->min) index++;
        }

        it->name = "W" + to_string(index);
    }

    for (auto x: bodies)
        result[x.JournalIdentifier] = x.name;

    return result;
}

bool blacklist(Body *body)
{
    // if any body is named SN_PART, only that body will be exported
    for ( Body *bd : *( dynamic_cast<Part*>( body->OwningPart() )->Bodies() ) )
    {
        if ( strcmp( bd->Name().GetText(), "SN_PART" ) == 0 )
        {
            // if SN_PART body is parameter body -> do NOT blacklist
            if ( bd == body )
                return false;
            
            // blacklist all other bodies
            return true;
        }
    }

    /*
        If the body name has any of the values from
            blacklisted_bodies in it, it will be blacklisted
        Else, not blacklisted
    */

    for ( const char *bd : BLACKLISTED_BODIES )
    {
        if ( strstr(body->Name().GetText(), bd) )
            return true;

        for ( Features::Feature *feature : body->GetFeatures() )
        {
            for ( Features::Feature *parent : feature->GetParents() )
            {
                if ( strstr(parent->GetFeatureName().GetText(), bd) )
                    return true;
            }
        }
    }

    for ( const char *bd : BLACKLISTED_BODIES )
    {
        if ( strstr(body->Name().GetText(), bd) )
            return true;
    }

    return false;
}

bool blacklist(Sketch *sketch)
{
    /*
        If the sketch name has any of the values from
            whitelisted_sketches in it, it will not be blacklisted
        Else, blacklisted
    */

    const char* sketch_name = sketch->Name().GetText();

    // not found
    if ( WHITELISTED_SKETCHES.find( sketch_name ) == WHITELISTED_SKETCHES.end() )
        return true;

    return false;
}

void set_layer(Sketch *sketch)
{
    const char* sketch_name = sketch->Name().GetText();

    // not in WHITELISTED_SKETCHES (unlikely)
    if ( WHITELISTED_SKETCHES.find( sketch_name ) == WHITELISTED_SKETCHES.end() )
        return;

    sketch->SetLayer( WHITELISTED_SKETCHES.find( sketch_name )->second );
}

int get_number_of_body_exports(Part *part)
{
    int number_of_bodies = 0;

    for ( Body *body: *( part->Bodies() ) )
    {
        if ( blacklist(body) )
            continue;

        number_of_bodies++;
    }

    return number_of_bodies;
}

string get_part_property(Part* part, const char* property_name)
{
    NXObject::AttributeInformation info_query;
    
    try
    {
        info_query = part->GetUserAttribute(property_name, NXObject::AttributeTypeString, -1);

        return info_query.StringValue.GetText();
    }

    // error -> return empty string
    catch (const exception &ex) { return ""; }
}

string get_part_property(Part* part, const vector<char*> property_names)
{
    string result;
    for (char* property_name : property_names)
    {
        result = get_part_property(part, property_name);
        if ( !is_empty_property(result) )
            return result;
    }

    return "";
}

bool is_empty_property(string &value)
{
    for (char &c: value)
    {
        switch (c)
        {
            case ' ':
            case 'X':
            case 'x':
                break;
            default:
                return false;
        }
    }

    return true;
}

bool startswith(string text, string find_in)
{
    return find_in.rfind(text, 0);
}