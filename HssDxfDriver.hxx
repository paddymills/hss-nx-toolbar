
#ifndef HSSDXFDRIVER_H
#define HSSDXFDRIVER_H

#include "DxfExportWorker.hxx"
#include "LogStream.hxx"

#include <map>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>

static const char* DXF_OUTPUT_DIR = "\\\\hssieng\\SNDataPrd\\DXF\\";
static const int ANNOTATION_LAYER = 4;

using namespace NXOpen;
using namespace std;

class HssDxfDriver
{
    private:
        Session* session;
        DxfExportWorker export_worker;
        map<string, string> annotations;
        LogStream log;

        bool dry_run;

    public:

        HssDxfDriver();
        
        void test();

        void process_open_parts();
        void process_parts(vector<const char*>);
        void process_part(Part*);
        void process_part(BasePart* part) { process_part( dynamic_cast<Part*>(part) ); }
        void process_part(const char*);

        void handle_part_properties(Part*);
        void handle_sketches(Part*);
        void handle_bodies(Part*);
        void handle_thickness(Body*);

};

#endif