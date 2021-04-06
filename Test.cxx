
#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"
#include "HssDxfDriverUtils.hxx"
#include "LogStream.hxx"

#include <map>
#include <fstream>

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Features_Feature.hxx>

using namespace NXOpen;
using namespace std;

void test1()
{
    DxfExportWorker *exporter;

    string PATH = "C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\";
    vector<int> process_type = { 0, 0, 1, 2, 2, 1 };

    // run all
    // process_type = vector<int>(process_type.size(), 3);
    
    vector<string> part_files{
        "1190181A_G1A-web_named.prt",
        "1190181A_G2A-web.prt",
        "1190259A_m3g.prt",
        "1190259A_SP1-b.prt",
        "1190259A_SP2-c.prt",
        "1190259A_x1b.prt"
    };

    exporter = new DxfExportWorker();

    int index = 0;
    for ( string p: part_files )
    {
        switch ( process_type[index++] )
        {
            case 0: // skip processing
                continue;
            
            case 1: // process, but don't export
                exporter->dry_run = true;
                break;
            
            case 2: // process and export
                exporter->dry_run = false;
                break;
            
            default: break;
        }

        try
        {
            exporter->process_part((PATH + p).c_str());
        }

        catch(const exception &ex)
        {
            exporter->log << "Exception caught: " << ex.what() << endl;
        }
    }

    delete exporter;
}

void test2()
{
    LogStream log;

    log << "test";
    log.increase_indent();
    log << "1" << endl;

    log << "test" << 2 << endl;

}

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    // test1();
    test2();
}
