
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include "DxfExportWorker.hxx"

#include <experimental/filesystem>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/BasePart.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCloseResponses.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/PartLoadStatus.hxx>
#include <NXOpen/DexBuilder.hxx>
#include <NXOpen/DexManager.hxx>
#include <NXOpen/DxfdwgCreator.hxx>

#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/CartesianCoordinateSystem.hxx>
#include <NXOpen/CoordinateSystemCollection.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Face.hxx>
#include <NXOpen/Features_DatumCsys.hxx>
#include <NXOpen/Features_DatumCsysBuilder.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>
#include <NXOpen/Xform.hxx>
#include <NXOpen/XformCollection.hxx>

#include <NXOpen/LogFile.hxx>
#include <NXOpen/LicenseManager.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/NXException.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

#include <NXOpen/UI.hxx>
#include <NXOpen/NXMessageBox.hxx>

using namespace NXOpen;
using namespace std;

namespace fs = experimental::filesystem;

/* Initialize static variables */
Session *(Dxf_Export_Worker::nx_session) = NULL;
LogFile *(Dxf_Export_Worker::nx_system_log) = NULL;

Dxf_Export_Worker::Dxf_Export_Worker()
{
    /* init class members */
    nx_session = Session::GetSession();
    nx_system_log = nx_session->LogFile();
    
    part = nullptr;
    dxf_factory = nullptr;
    selected_objects = nullptr;
}

Dxf_Export_Worker::~Dxf_Export_Worker()
{
    /* close factory, if not null */
    if (dxf_factory)
        dxf_factory->Destroy();
        delete selected_objects;
        delete dxf_factory;

    /* cleanup members */
    // delete nx_session;
    // delete nx_system_log;
    // if (part)
    //     delete part;
}

void Dxf_Export_Worker::init_factory() {
    /* init dxf/dwg exporter */
    dxf_factory = nx_session->DexManager()->CreateDxfdwgCreator();

    dxf_factory->SetSettingsFile(DXF_EXPORT_CONFIG);
    dxf_factory->SetExportData(DxfdwgCreator::ExportDataOptionDrawing);
    dxf_factory->SetViewEditMode(true);
    dxf_factory->SetFlattenAssembly(true);
    dxf_factory->SetExportAs(DxfdwgCreator::ExportAsOptionThreeD);
    dxf_factory->ExportSelectionBlock()->SetSelectionScope(ObjectSelector::ScopeSelectedObjects);
    dxf_factory->SetExportFacesAs(DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    dxf_factory->SetProcessHoldFlag(true);
    
    selected_objects = dxf_factory->ExportSelectionBlock()->SelectionComp();
}

void Dxf_Export_Worker::process_part(const char *part_file_name)
{
    /* Open part */
    PartLoadStatus *part_load_status;
    part = dynamic_cast<Part *>(Dxf_Export_Worker::nx_session->Parts()->OpenActiveDisplay(part_file_name, DisplayPartOptionReplaceExisting, &part_load_status));
    delete part_load_status;

    init_factory();

    /* set up dxf/dwg export for file */
    dxf_factory->SetInputFile(part_file_name);

    nx_session->ApplicationSwitchImmediate("UG_APP_MODELING");

    // ********************
    // *     DO STUFF     *
    // ********************
    add_sketches();
    export_bodies();
    

    /* reset dxf/dwg exporter object selection */
    dxf_factory->Destroy();
    selected_objects = nullptr;
    dxf_factory = nullptr;

    /* close part */
    part->Close(BasePart::CloseWholeTreeTrue, BasePart::CloseModifiedCloseModified, nullptr);
    
    nx_session->ApplicationSwitchImmediate("UG_APP_NOPART");
    
    /* clean up part objects */
    delete part;
}

void Dxf_Export_Worker::add_sketches()
{
    nx_system_log->WriteLine("\n***********************");
    nx_system_log->WriteLine(  "*   Adding Sketches   *");
    nx_system_log->WriteLine(  "***********************\n");

    /* Add ZINC sketches */
    for (Sketch *sketch: *(part->Sketches()))
    {
        /* add ZINC sketches */
        if (strstr(sketch->Name().GetText(), "ZINC"))
        {
            /* sketch is hidden: skip */
            if (sketch->IsBlanked())
            {
                nx_system_log->Write(" - Skipping blanked sketch: ");
                nx_system_log->Write(sketch->Name().GetText());
            }

            /* sketch is visible: add to file */
            else
            {
                nx_system_log->Write(" + Adding sketch: ");
                nx_system_log->Write(sketch->Name().GetText());
                nx_system_log->Write(" -> ");

                /* add sketch lines to sketches */
                if (selected_objects->Add(sketch->GetAllGeometry()))
                    nx_system_log->Write("OK");
                else
                    nx_system_log->Write("FAILED");
            }

            nx_system_log->WriteLine("");
        }
    }

}

void Dxf_Export_Worker::export_bodies()
{
    nx_system_log->WriteLine("\n***********************");
    nx_system_log->WriteLine(  "*    Adding Bodies    *");
    nx_system_log->WriteLine(  "***********************\n");

    /* build base part file name */
    string base_save_file_name(DXF_OUTPUT_DIR);
    base_save_file_name.append(part->GetStringAttribute("JobNo").GetText());
    base_save_file_name.append("_");
    base_save_file_name.append(part->GetStringAttribute("Mark").GetText());
    base_save_file_name.append("-");

    string body_name;
    Point *p = nullptr;
    double min_z = 0.0;

    /* Add body to dxf export */
    for (Body *body: *(part->Bodies()))
    {
        // TODO: Body name inference if empty
        /* build file name */
        body_name.assign(body->Name().GetText());
        
        /* Set output file name */
        /* {DXF_OUTPUT_DIR}\{Job}_{Girder}-{BodyName}.dxf */
        dxf_factory->SetOutputFile(base_save_file_name + body_name + ".dxf");

        /* export body */
        nx_system_log->Write("Adding body: ");
        nx_system_log->WriteLine(body_name);

        nx_system_log->WriteLine(" + Faces:");
        for (Face *f: body->GetFaces())
        {
            if (f->GetEdges().size() > 4)
            {
                nx_system_log->Write("   + ");
                nx_system_log->Write(f->JournalIdentifier().GetText());
                nx_system_log->Write(": Edge Count=");
                nx_system_log->WriteLine(to_string(f->GetEdges().size()));

                for (Edge *e: f->GetEdges())
                {
                    try
                    {
                        p = part->Points()->CreatePoint(e, SmartObject::UpdateOptionWithinModeling);

                        min_z = min(p->Coordinates().Z, min_z);

                        // part->Points()->DeletePoint(p);
                    }
                    catch (const exception &ex){}
                }
            }
        }

        if (min_z != 0.0)
            set_wcs_to_face(min_z);

        /* add body to export */
        bool added = selected_objects->Add(body);

        /* 
            delete part file if it exists
            this seems to speed up export compared to overwriting files
            also, it keeps from accumulating *_bk.dxf files
        */
        if (fs::exists(fs::path(dxf_factory->OutputFile().GetText())))
            remove(dxf_factory->OutputFile().GetText());
            
        /* generate DXF file */
        NXObject *generate_result = dxf_factory->Commit();
        
        /* delete added body (so that it does not export next time) */
        selected_objects->Remove(body);
    }
}

void Dxf_Export_Worker::set_wcs_to_face(double new_z)
{
    Features::Feature *nullNXOpen_Features_Feature(NULL);
    Features::DatumCsysBuilder *datum_csys_builder = part->Features()->CreateDatumCsysBuilder(nullNXOpen_Features_Feature);
    
    Point3d origin(0.0, 0.0, new_z);
    Vector3d x_dir(1.0, 0.0, 0.0);
    Vector3d y_dir(0.0, 1.0, 0.0);
    Xform *xform = part->Xforms()->CreateXform(origin, x_dir, y_dir, SmartObject::UpdateOptionWithinModeling, 1.0);

    CartesianCoordinateSystem *ccord_sys = part->CoordinateSystems()->CreateCoordinateSystem(xform, SmartObject::UpdateOptionWithinModeling);

    /* create datum CSYS */
    datum_csys_builder->SetCsys(ccord_sys);
    NXObject *csys_commit = datum_csys_builder->Commit();

    Features::DatumCsys *datum_csys(dynamic_cast<NXOpen::Features::DatumCsys *>(csys_commit));
    datum_csys->SetWcsAtCsys();

    /* clean up pointers and builder */
    delete xform;
    delete ccord_sys;
    delete csys_commit;
    datum_csys_builder->Destroy();
}

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    Dxf_Export_Worker *exporter;

    try
    {
        exporter = new Dxf_Export_Worker();
        const char *test_part = "C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt";
        
        Dxf_Export_Worker::nx_system_log->WriteLine("\n\t*************************************************************");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*                                                           *");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*                     NXOpen Dxf Export                     *");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*                                                           *");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*************************************************************\n");

        Dxf_Export_Worker::nx_session->LicenseManager()->Reserve("solid_modeling", nullptr);
        exporter->process_part(test_part);
        Dxf_Export_Worker::nx_session->LicenseManager()->Release("solid_modeling", nullptr);
        
        Dxf_Export_Worker::nx_system_log->WriteLine("\n\t*************************************************************");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*                                                           *");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*                            End                            *");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*                                                           *");
        Dxf_Export_Worker::nx_system_log->WriteLine(  "\t*************************************************************\n");
    }

    catch(const exception &ex)
    {
        Dxf_Export_Worker::nx_system_log->WriteLine(ex.what());
        
        // UI *user_interface = UI::GetUI();
        // user_interface->NXMessageBox()->Show("Block Styler", NXMessageBox::DialogTypeError, ex.what());
        // delete user_interface;
    }

    delete exporter;
}