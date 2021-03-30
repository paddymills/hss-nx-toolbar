
#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"
#include "HssDriverUtils.hxx"

#include <map>

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
    vector<string> part_files{
        // "1190181A_G1A-web_named.prt",
        // "1190181A_G2A-web.prt",
        "1190259A_m3g.prt",
        "1190259A_SP2-c.prt",
        "1190259A_x1b.prt"
    };

        exporter = new DxfExportWorker();
        // exporter->dry_run = true;

        for (string p: part_files)
        {
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
    DxfExportWorker *exporter;

    string PATH = "C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\1190181A_G1A-web_named.prt";

    try
    {
        exporter = new DxfExportWorker();

        map<string, string> names = get_web_names(exporter->nx_session->Parts()->Work());

        
        exporter->log << "reading body map" << endl;
        for (Body *body: *(exporter->nx_session->Parts()->Work()->Bodies()))
        {
            exporter->log << " + " << body->JournalIdentifier().GetText();
            exporter->log << ": " << names[body->JournalIdentifier().GetText()] << endl;
        }
    }

    catch(const exception &ex)
    {
        exporter->log << "Exception caught: " << ex.what() << endl;
    }

    delete exporter;
}

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    test1();
    // test2();
}
