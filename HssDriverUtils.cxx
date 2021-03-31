
#include "BodyBoundary.hxx"

#include <string>
#include <map>

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

// strings to be found in sketch names
const vector<char*> WHITELISTED_SKETCHES = { "ZINC", "DOR", "NOCUT", "NO_CUT" };
const vector<int> WHITELISTED_SKETCHES_LAYERS = { 2, 3, 3, 3 };

// list of strings to be found in body names
//  or names of features that construct body
const vector<char*> BLACKLISTED_BODIES = { "SHIM" };

struct BodyMinMax {
    string JournalIdentifier;
    double min, max;
    bool is_parent;
    string name;
};

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

    for (const char *sk : WHITELISTED_SKETCHES)
    {
        // basically `sk is in sketch name`
        if ( strstr(sketch->Name().GetText(), sk) )
            return false;
    }

    return true;
}

void set_layer(Sketch *sketch)
{
    int i = 0;
    for ( const char *sk : WHITELISTED_SKETCHES )
    {
        // basically `sk is in sketch name`
        if ( strstr(sketch->Name().GetText(), sk) )
        {
            sketch->SetLayer( WHITELISTED_SKETCHES_LAYERS[i] );
            break;
        }

        i++;
    }
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

bool startswith(string text, string find_in)
{
    return find_in.rfind(text, 0);
}