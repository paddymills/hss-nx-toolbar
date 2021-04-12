
#ifndef NXDXFEXPORT_H
#define NXDXFEXPORT_H

#include <map>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/DxfdwgCreator.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

using namespace NXOpen;
using namespace std;

class NxDxfExport
{
    private:
        DxfdwgCreator* _nxDxf;
        vector<NXObject*> purgeable_objects;

        bool add(NXObject* obj) { return _nxDxf->ExportSelectionBlock()->SelectionComp()->Add(obj); }
        bool add(vector<NXObject*> objs) { return _nxDxf->ExportSelectionBlock()->SelectionComp()->Add(objs); }
        bool remove(NXObject* obj) { return _nxDxf->ExportSelectionBlock()->SelectionComp()->Remove(obj); }
        void purge_objects();

    public:
        NxDxfExport();
        ~NxDxfExport() { _nxDxf->Destroy(); }
        void init_settings();

        void set_input_file(Part* p) { _nxDxf->SetInputFile(p->FullPath()); }
        void set_output_file(const char* o) { _nxDxf->SetOutputFile( o ); }
        
        bool add_sketch(Sketch*);
        bool export_body(Body*);
};

#endif
