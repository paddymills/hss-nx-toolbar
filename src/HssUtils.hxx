
#ifndef HSSUTILS_H
#define HSSUTILS_H

#include <map>
#include <regex>

#include <uf_defs.h>
#include <NXOpen/Body.hxx>
#include <NXOpen/Sketch.hxx>

using namespace NXOpen;
using namespace std;

bool blacklist(Sketch*);
bool blacklist(Body*);
int matches_regex(NXString, map<string, int>);
bool matches_regex(NXString, vector<string>);
bool matches_regex(NXString, regex);

#endif