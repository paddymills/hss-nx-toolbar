
#include "DxfExportWorker.hxx"

#include <uf_defs.h>
#include <NXOpen/Session.hxx>

using namespace NXOpen;
using namespace std;

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
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
