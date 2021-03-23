
#include "BodyBoundary.hxx"

#include <string>
#include <map>

#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>

using namespace NXOpen;
using namespace std;

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