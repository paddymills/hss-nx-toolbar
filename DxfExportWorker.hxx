
#ifndef DXFEXPORTWORKER_H
#define DXFEXPORTWORKER_H

#include <fstream>
#include <map>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/DxfdwgCreator.hxx>
#include <NXOpen/LogFile.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

using namespace NXOpen;
using namespace std;

static const char* DXF_EXPORT_CONFIG = "C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def";

static const double NOTE_SIZE = 5.0;
static const double NOTE_OFFSET = 10.0;

static const int PROFILE_LAYER = 1;

class DxfExportWorker
{
    private:
        Session* session;
        DxfdwgCreator* dxf_factory;

        vector<NXObject*> purgeable_objects;
        double anno_x, anno_y;

        bool add_object_to_export(NXObject* obj) { return dxf_factory->ExportSelectionBlock()->SelectionComp()->Add(obj); }
        bool add_object_to_export(vector<NXObject*> objs) { return dxf_factory->ExportSelectionBlock()->SelectionComp()->Add(objs); }
        bool add_purgeable_object_to_export(NXObject*);
        void purge_objects();

    public:
        DxfExportWorker();
        ~DxfExportWorker();

        void set_input_file(Part* p) { dxf_factory->SetInputFile(p->FullPath().GetText()); }
        void set_output_file(const char* o) { dxf_factory->SetOutputFile( o ); }
        void set_annotation_xy(double, double);

        void init_factory();
        void rem_factory() { dxf_factory->Destroy(); }

        bool add_sketch(Sketch* sk) { return add_object_to_export( sk->GetAllGeometry() ); }
        bool export_body(Body*);
        NXObject* add_annotations(map<string, string>, double, double);
        NXObject* add_annotations(map<string, string>);
        NXObject* add_annotations(vector<NXString>, double, double);
        NXObject* add_annotations(vector<NXString>);

};

#endif
