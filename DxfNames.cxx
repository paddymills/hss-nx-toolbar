
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include "DxfNames.hxx"

#include "BodyBoundary.hxx"
#include "DxfExportWorker.hxx"
#include "HssDxfDriverUtils.hxx"

#include <map>
#include <regex>
#include <experimental/filesystem>

#include <uf.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>

using namespace NXOpen;
using namespace std;
using namespace regex_constants;

namespace fs = experimental::filesystem;

const vector<char*> JOB_PROP_KEYS = { "JobNo", "JOB_NUMBER" };
const vector<char*> MARK_PROP_KEYS = { "Mark", "PIECE_MARK" };
const string WEB_PATTERN = "(W[0-9]+)+";
const string WEB_PARENT_PATTERN = "(W[0-9]+){2,}";

map<Body*, string> get_export_names(Part* part)
{
    map<Body*, string> result;
    string part_name;

    // get filename (no directories or extensions)
    fs::path *part_path = new fs::path(part->FullPath().GetText());
    string filename(part_path->filename().stem().string());

    // build base part file name
    part_name.append(get_part_property( part, JOB_PROP_KEYS ));
    part_name.append("_");
    part_name.append(get_part_property( part, MARK_PROP_KEYS ));

    // make sure part filename starts with build part_name
    // or part_name is more than just job_
    // could be stale data
    if ( startswith(part_name, filename) && part_name.length() > 10 )
        part_name = filename;

    /* 
        **********************************************
        *    Part name map from part type            *
        **********************************************
    */
    switch ( get_part_type(part) )
    {
        case PartType::Web :
        case PartType::Flange :
            result = get_web_names(part);
            for ( pair<Body*, string> it : result )
                result[ it.first ] = part_name + "-" + it.second;

            break;

        case PartType::SolePlate :
        case PartType::SingleBodyPart :
            for ( Body* body : *( part->Bodies() ) )
            {
                if ( !blacklist( body ) )
                    result[ body ] = part_name;
            }

            break;

        case PartType::MultiBodyPart :
            int i = 1;
            for ( Body* body : *( part->Bodies() ) )
            {
                if ( !blacklist( body ) )
                    result[ body ] = part_name + "_" + to_string(i++);
            }
    }

    return result;
}

PartType get_part_type(Part* part)
{
    regex web_re(WEB_PATTERN, ECMAScript | icase);
    cmatch m;
    
    // get filename (no directories or extensions)
    fs::path *part_path = new fs::path(part->FullPath().GetText());
    string filename(part_path->filename().stem().string());

    /* 
        ******************************
        *    Web                     *
        ******************************
    */
    for ( Body* body: *(part->Bodies()) )
    {
        regex_match( body->Name().GetText(), m, web_re );
        if ( !m.empty() )
            return PartType::Web;
    }
    
    if ( filename.find("web") != string::npos )
        return PartType::Web;

    /* 
        ******************************
        *    Flange                  *
        ******************************

        not yet implemented: flanges should be same
            as single body
    */

    /* 
        ******************************
        *    Sole plate              *
        ******************************

        not yet implemented: sole plates should be same
            as single body
    */

    /* 
        ******************************
        *    Single body             *
        ******************************

        could be single body or multiple bodies
            with all but one blacklisted
    */
    if ( get_number_of_body_exports(part) == 1 )
        return PartType::SingleBodyPart;

    /* 
        ******************************
        *    Multiple body           *
        ******************************

        multiple bodies with no body named `SN_PART`
        will be exported _1, _2, _3, etc.
    */
    return PartType::MultiBodyPart;
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

map<Body*, string> get_web_names(Part *part)
{
    regex web_re(WEB_PARENT_PATTERN, ECMAScript | icase);
    smatch m;

    map<Body*, string> result;

    vector<BodyMinMax> bodies;
    BodyBoundary *bound;
    BodyMinMax *bmm;

    for ( Body* body: *(part->Bodies()) )
    {
        bound = new BodyBoundary(body);

        bmm = new BodyMinMax();
        bmm->JournalIdentifier = body->JournalIdentifier().GetText();
        bmm->min = bound->minimum('x');
        bmm->max = bound->maximum('x');
        bmm->is_parent = false;
        bmm->name = body->Name().GetText();
        bmm->body = body;

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
    for ( auto it = bodies.begin(); it != bodies.end(); it++ )
    {
        regex_match( it->name, m, web_re );
        if ( !m.empty() )
        {
            it->is_parent = true;
            break;
        }

        if (it->min == min_x && it->max == max_x)
        {
            it->is_parent = true;

            for (int i=1; i<bodies.size(); i++)
                it->name.append("W" + to_string(i));

            break;
        }
    }
    
    // name child bodies
    for ( auto it = bodies.begin(); it != bodies.end(); it++ )
    {
        if ( it->is_parent || it->name.length() > 0 )
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
        result[ x.body ] = x.name;

    return result;
}