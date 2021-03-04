
#ifndef DXFEXPORTWORKER
#define DXFEXPORTWORKER

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/DxfdwgCreator.hxx>
#include <NXOpen/LogFile.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

using namespace NXOpen;

static const char *DXF_EXPORT_CONFIG = "C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def";
static const char *DXF_OUTPUT_DIR = "\\\\hssieng\\SNDataPrd\\DXF\\";

class DxfExportWorker
{
    private:

        DxfdwgCreator *dxf_factory;
        SelectNXObjectList *selected_objects;
        Part *part;

    public:

        static Session *nx_session;
        static LogFile *nx_system_log;
        DxfExportWorker();
        ~DxfExportWorker();

        void init_factory();
        void process_part();
        void process_part(const char*);
        void export_bodies();
        void add_sketches();
        void set_wcs_to_face(double);

};

#endif
