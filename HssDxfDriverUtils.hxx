
#ifndef HSSDRIVERUTILS_H
#define HSSDRIVERUTILS_H

#include <map>

#include <uf_defs.h>
#include <NXOpen/Part.hxx>

using namespace NXOpen;
using namespace std;

bool blacklist(Body*);
bool blacklist(Sketch*);
void set_layer(Sketch*);

string get_part_property(Part*, const char*);
string get_part_property(Part*, const vector<char*>);
bool is_empty_property(string);

// things missing from c++
bool startswith(string, string);
bool startswith(string, NXString);

#endif