
#ifndef DXFEXPORTWORKER_H
#define DXFEXPORTWORKER_H

#include <fstream>
#include <map>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/DxfdwgCreator.hxx>
#include <NXOpen/LogFile.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

using namespace NXOpen;
using namespace std;

static const char *DXF_EXPORT_CONFIG = "C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def";
static const char *DXF_OUTPUT_DIR = "\\\\hssieng\\SNDataPrd\\DXF\\";
static const char *LOG_DIR = "C:\\Users\\PMiller1\\git\\nx-dxf\\logs\\";

static const double NOTE_SIZE = 5.0;
static const double NOTE_OFFSET = 10.0;

class DxfExportWorker
{
    private:

        DxfdwgCreator *dxf_factory;
        Part *part;
        map<string, string> body_names;
        int body_index;

        vector<NXObject*> purgeable_objects;
        map<string, string> annotations;

        bool add_object_to_export(NXObject*);
        bool add_object_to_export(vector<NXObject*>);
        bool add_purgeable_object_to_export(NXObject*);
        void purge_objects();

        bool is_empty_property(string&);
        string get_export_name(Body*);
        string get_part_property(string);
        string get_part_property(vector<string>);

    public:

        static Session *nx_session;
        static LogFile *nx_system_log;
        
        bool dry_run;
        ofstream log;
        
        DxfExportWorker();
        ~DxfExportWorker();

        void init_factory();

        void process_part();
        void process_part(const char*);

        void handle_part_properties();
        void add_sketches();
        void export_bodies();
        NXObject *add_annotations(double, double);

        void handle_body(Body*);
        void set_wcs_to_face(double);

};

#endif
