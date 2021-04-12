
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

using namespace std;

void remove_if_exists(const char* filename);
void clear_dir_except(string dir, const char* file);
string filename(const char* full_path);

#endif