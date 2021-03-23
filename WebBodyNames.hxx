#ifndef WEBBODYNAMES_H
#define WEBBODYNAMES_H

#include <string>
#include <map>

#include <uf_defs.h>
#include <NXOpen/Part.hxx>

using namespace NXOpen;
using namespace std;

struct BodyMinMax {
    string JournalIdentifier;
    double min, max;
    bool is_parent;
    string name;
};

map<string, string> get_web_names(Part*);

#endif