
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"

#include <map>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/Features_Feature.hxx>

using namespace NXOpen;
using namespace std;

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
    return find_in.find(text, 0) == 0;
}

bool startswith(string text, NXString find_in)
{
    return startswith( text, string( find_in.GetText() ) );
}