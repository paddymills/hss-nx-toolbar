
#include "config.hxx"
#include "HssUtils.hxx"

#include <string>
#include <vector>
#include <regex>

#include <uf_defs.h>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/Sketch.hxx>


using namespace NXOpen;
using namespace std;

bool blacklist(Sketch* sketch)
{
    /*
        If the sketch name has any of the values from
            whitelisted_sketches in it, it will not be blacklisted
        Else, blacklisted
    */

    // no sketch regexes match sketch name
    if ( matches_regex( sketch->Name(), config::WHITELISTED_SKETCHES ) == -1 )
        return true;

    return false;
}

bool blacklist(Body* body)
{
    // whitelist if named for single body export
    if ( body->Name().GetText() == config::SINGLE_BODY_EXPORT_NAME )
        return false;

    for ( Body* bd : *( dynamic_cast<Part*>( body->OwningPart() )->Bodies() ) )
        if ( bd->Name().GetText() == config::SINGLE_BODY_EXPORT_NAME )
            // blacklist all other bodies
            return true;

    /*
        If the body name has any of the values from
            blacklisted_bodies in it, it will be blacklisted
        Else, not blacklisted
    */

    regex _regex;
    for ( string bd : config::BLACKLISTED_BODIES )
    {
        _regex = regex( bd );

        // body name matches regex
        if ( matches_regex( body->Name(), _regex ) )
            return true;

        // features that make up body match regex
        for ( Features::Feature* feature : body->GetFeatures() )
            for ( Features::Feature* parent : feature->GetParents() )
                if ( matches_regex( parent->GetFeatureName(), _regex ) )
                    return true;
    }

    return false;
}

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