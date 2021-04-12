
#ifndef NXUTILS_H
#define NXUTILS_H

#include <uf_defs.h>
#include <NXOpen/Part.hxx>

using namespace NXOpen;
using namespace std;

string get_part_property(Part*, const char*);
string get_part_property(Part*, const vector<char*>);
bool is_empty_property(string);

void set_top_view(Part*);

#endif