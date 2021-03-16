
#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"

#include <iostream>
#include <fstream>
#include <iomanip>

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


    log.open("C:\\Users\\PMiller1\\git\\nx-dxf\\log.txt");
    try
    {
        exporter = new DxfExportWorker();

        for (Body *body: *(exporter->nx_session->Parts()->Work()->Bodies()))
        {
            log << "got Body: ";
            log << body->Name().GetText();
            log << endl;


            // Point *p = nullptr;
            // for (Edge *e: body->GetEdges())
            // {
            //     try
            //     {
            //         // create center point on edge
            //         p = body->OwningPart()->Points()->CreatePoint(e, SmartObject::UpdateOptionWithinModeling);

            //         log << " * ( ";
            //         log << to_string(p->Coordinates().X);
            //         log << ", ";
            //         log << to_string(p->Coordinates().Y);
            //         log << ", ";
            //         log << to_string(p->Coordinates().Z);
            //         log << " ) --- [ ";
            //         log << e->JournalIdentifier().GetText();
            //         log << " ]" << endl;
            //     }

            //     // some points will error out, don't care
            //     catch (const exception &ex) {}
            // }

            for (Features::Feature *f: body->GetFeatures())
            {
                if ( string(f->FeatureType().GetText()).find("HOLE", 0) != string::npos )
                    continue;

                log << " + ";
                log << f->FeatureType().GetText();
                log << " : ";
                log << f->GetFeatureName().GetText();
                log << endl;

                log << "\tParents:" << endl;
                for (Features::Feature *pf: f->GetParents())
                {
                    log << "\t + ";
                    log << pf->GetFeatureName().GetText();
                    log << " | ";
                    log << pf->Name().GetText();
                    log << endl;
                }

                log << "\tChildren:" << endl;
                for (Features::Feature *pf: f->GetChildren())
                {
                    log << "\t + ";
                    log << pf->GetFeatureName().GetText();
                    log << " | ";
                    log << pf->Name().GetText();
                    log << endl;
                }

                log << "\tBodies:" << endl;
                for (Body *fb: f->GetBodies())
                {
                    log << "\t + ";
                    log << fb->Name().GetText();
                    log << " (" << fb->JournalIdentifier().GetText() << ")";
                    log << endl;
                }
            }

            log << "\n  ----------------------------------  \n" << endl;
        }
    }

    catch(const exception &ex)
    {
        DxfExportWorker::nx_system_log->WriteLine(ex.what());
    }

    log.close();
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
