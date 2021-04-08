
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include "HssDxfDriverUtils.hxx"

#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"

#include <map>
#include <regex>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/Features_Feature.hxx>

using namespace NXOpen;
using namespace std;
using namespace regex_constants;

// strings to be found in sketch names
const map<string, int> WHITELISTED_SKETCHES {
        { "ZINC"     , 2 },
        { "DOR"      , 3 },
        { "NOCUT"    , 3 },
        { "NO_CUT"   , 3 }
    };

// list of strings to be found in body names
//  or names of features that construct body
const vector<string> BLACKLISTED_BODIES = { "SHIM" };

int matches_regex(NXString str, map<string, int> regex_map)
{
    regex _regex;
    for ( pair<string, int> x : regex_map)
    {
        _regex = regex( x.first );

        if ( matches_regex( str, _regex ))
            return x.second;
    }

    return -1;
}

bool matches_regex(NXString str, vector<string> regex_vec)
{
    regex _regex;

    for ( string x : regex_vec)
    {
        _regex = regex( x );

        if ( matches_regex( str, _regex ))
            return true;
    }

    return false;
}

bool matches_regex(NXString str, regex r)
{
    cmatch m;

    return regex_search( str.GetText(), m, r );
}

bool blacklist(Body* body)
{
    // if body is named SN_PART, only that body will be exported
    if ( body->Name().GetText() == "SN_PART" )
        return false;

    for ( Body* bd : *( dynamic_cast<Part*>( body->OwningPart() )->Bodies() ) )
    {
        if ( bd->Name().GetText() == "SN_PART" )
            // blacklist all other bodies
            return true;
    }

    /*
        If the body name has any of the values from
            blacklisted_bodies in it, it will be blacklisted
        Else, not blacklisted
    */

    regex _regex;
    for ( string bd : BLACKLISTED_BODIES )
    {
        _regex = regex( bd );

        if ( matches_regex( body->Name(), _regex ) )
            return true;

        for ( Features::Feature* feature : body->GetFeatures() )
        {
            for ( Features::Feature* parent : feature->GetParents() )
            {
                if ( matches_regex( parent->GetFeatureName(), _regex ) )
                    return true;
            }
        }
    }

    return false;
}

bool blacklist(Sketch* sketch)
{
    /*
        If the sketch name has any of the values from
            whitelisted_sketches in it, it will not be blacklisted
        Else, blacklisted
    */

    // no sketch regexes match sketch name
    if ( matches_regex( sketch->Name(), WHITELISTED_SKETCHES ) == -1 )
        return true;

    return false;
}

void set_layer(Sketch* sketch)
{
    int layer = matches_regex( sketch->Name(), WHITELISTED_SKETCHES );
    
    if ( layer > -1 )
        sketch->SetLayer(layer);
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
    catch (const exception& ex) { return ""; }
}

string get_part_property(Part* part, const vector<char*> property_names)
{
    string result;
    for (const char* property_name : property_names)
    {
        result = get_part_property(part, property_name);
        if ( !is_empty_property(result) )
            return result;
    }

    return "";
}

bool is_empty_property(string value)
{
    for (char& c: value)
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
