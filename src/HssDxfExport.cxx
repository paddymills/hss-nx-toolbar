
#include "HssPartProcessing.hxx"

#include <map>

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/LogFile.hxx>

using namespace NXOpen;
using namespace std;

enum TestType { Skip, DryRun, Export };

void tests();

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char* param, int* retCode, int paramLen)
{
    tests();
}

void tests()
{
    HssPartProcessing driver;

    string PATH = "C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\";
    map<string, TestType> part_files{
        { "1190181A_G1A-web_named.prt", TestType::DryRun },
        { "1190181A_G2A-web.prt",       TestType::DryRun },
        { "1190259A_m3g.prt",           TestType::DryRun },
        { "1190259A_SP1-b.prt",         TestType::DryRun },
        { "1190259A_SP2-c.prt",         TestType::DryRun },
        { "1190259A_x1b.prt",           TestType::DryRun }
    };

    Session::GetSession()->LogFile()->WriteLine("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DXF Export ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

    for ( pair<string, int> t : part_files )
    {
        switch ( t.second )
        {
            case TestType::Skip   : // skip processing
                continue;
            
            case TestType::DryRun : // process, but don't export
                driver.dry_run = true;
                break;
            
            case TestType::Export : // process and export
                driver.dry_run = false;
                break;
        }

        driver.process_part( (PATH + t.first) );
    }

    Session::GetSession()->LogFile()->WriteLine("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DXF End    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}