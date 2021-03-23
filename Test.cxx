
#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"
#include "WebBodyNames.hxx"

#include <fstream>
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
    vector<string> part_files{ "1190181A_G1A-web_named.prt", "1190181A_G1A-web.prt" };

    try
    {
        exporter = new DxfExportWorker();

        for (string p: part_files)
            exporter->process_part((PATH + p).c_str());
    }

    catch(const exception &ex)
    {
        DxfExportWorker::nx_system_log->WriteLine(ex.what());
    }

    delete exporter;
}

void test2()
{
    DxfExportWorker *exporter;

    string PATH = "C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\1190181A_G1A-web_named.prt";

    ofstream log;
    log.open("C:\\Users\\PMiller1\\git\\nx-dxf\\test.log");

    try
    {
        exporter = new DxfExportWorker();

        map<string, string> names = get_web_names(exporter->nx_session->Parts()->Work());

        
        log << "reading body map" << endl;
        for (Body *body: *(exporter->nx_session->Parts()->Work()->Bodies()))
        {
            log << " + " << body->JournalIdentifier().GetText();
            log << ": " << names[body->JournalIdentifier().GetText()] << endl;
        }
        log.close();
    }

    catch(const exception &ex)
    {
        DxfExportWorker::nx_system_log->WriteLine(ex.what());
    }

    delete exporter;
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
