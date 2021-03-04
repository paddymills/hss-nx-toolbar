
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

    try
    {
        exporter = new DxfExportWorker();
        const char *test_part = "C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt";

        exporter->process_part(test_part);
    }

    catch(const exception &ex)
    {
        DxfExportWorker::nx_system_log->WriteLine(ex.what());
    }

    delete exporter;
}
