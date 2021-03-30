#ifndef HSSDRIVERUTILS_H
#define HSSDRIVERUTILS_H

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
bool blacklist(Body*);
bool blacklist(Sketch*);
int get_number_of_body_exports(Part*);

// things missing from c++
bool startswith(string, string);

#endif