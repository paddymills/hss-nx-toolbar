
#include "filesystem.hxx"

#include <string>
#include <experimental/filesystem>

using namespace std;
namespace fs = experimental::filesystem;

void remove_if_exists(const char* filename)
{
    if ( fs::exists( fs::path( filename ) ) )
            remove( filename );
}

void clear_dir_except(string dir, const char* file)
{
    for ( fs::directory_entry e : fs::directory_iterator( dir ) )
        if ( e.path() != file )
            fs::remove( e.path() );
}

string filename(const char* full_path)
{
    fs::path* part_path = new fs::path( full_path );
    
    return part_path->filename().stem().string();
}