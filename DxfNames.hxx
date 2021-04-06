
#ifndef DXFNAMES_H
#define DXFNAMES_H

#include <map>

#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>

using namespace NXOpen;
using namespace std;

enum PartType
{
    Web,
    Flange,
    SolePlate,
    SingleBodyPart,
    MultiBodyPart
};

struct BodyMinMax {
    string JournalIdentifier;
    double min, max;
    bool is_parent;
    string name;
    Body* body;
};

map<Body*, string> get_export_names(Part*);
PartType get_part_type(Part*);
int get_number_of_body_exports(Part*);
map<Body*, string> get_web_names(Part*);

#endif
