
#include <vector>
#include <sstream>

#include <uf_defs.h>
#include <NXOpen/NXException.hxx>
#include <NXOpen/Session.hxx>
#include <NXOpen/BaseCreator.hxx>
#include <NXOpen/BasePart.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Builder.hxx>
#include <NXOpen/DexBuilder.hxx>
#include <NXOpen/DexManager.hxx>
#include <NXOpen/DxfdwgCreator.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_SketchFeature.hxx>
#include <NXOpen/Line.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/ObjectSelector.hxx>
#include <NXOpen/ObjectTypeSelector.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCloseResponses.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/PartLoadStatus.hxx>
#include <NXOpen/PreviewBuilder.hxx>
#include <NXOpen/SelectNXObjectList.hxx>
#include <NXOpen/SelectObjectList.hxx>
#include <NXOpen/Session.hxx>
#include <NXOpen/Sketch.hxx>

#include <uf.h>
#include <NXOpen/NXException.hxx>
#include <NXOpen/LogFile.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Face.hxx>

using namespace NXOpen;
using namespace std;

void run(Session *nx_session)
{
    char *part_file_name = "C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt";
    char *dxf_output_dir = "\\\\hssieng\\SNDataPrd\\DXF\\";
    bool added;

    LogFile *log = nx_session->LogFile();

    /* Open part */
    PartLoadStatus *part_load_status;
    Part *part = dynamic_cast<Part *>(nx_session->Parts()->OpenActiveDisplay(part_file_name, DisplayPartOptionReplaceExisting, &part_load_status));
    delete part_load_status;
    
    nx_session->ApplicationSwitchImmediate("UG_APP_MODELING");
    
    /* DWG/DXF exporter */
    DxfdwgCreator *dxf_gen = nx_session->DexManager()->CreateDxfdwgCreator();
    dxf_gen->SetSettingsFile("C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def");
    dxf_gen->SetExportData(DxfdwgCreator::ExportDataOptionDrawing);
    dxf_gen->SetViewEditMode(true);
    dxf_gen->SetFlattenAssembly(true);
    dxf_gen->SetExportAs(DxfdwgCreator::ExportAsOptionThreeD);
    dxf_gen->ExportSelectionBlock()->SetSelectionScope(ObjectSelector::ScopeSelectedObjects);
    dxf_gen->SetExportFacesAs(DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    dxf_gen->SetProcessHoldFlag(true);
    
    dxf_gen->SetInputFile(part_file_name);
    
    /* Add ZINC sketches */
    vector<NXObject *> sketches;
    vector<NXObject *> sketch_geo;

    log->WriteLine("-----------------------");
    log->WriteLine("    Adding Sketches");
    log->WriteLine("-----------------------");
    for (Sketch *sketch: *(part->Sketches()))
    {
        /* add ZINC sketches */
        if (strstr(sketch->Name().GetText(), "ZINC"))
        {
            /* sketch is hidden: skip */
            if (sketch->IsBlanked())
            {
                log->Write(" - Skipping blanked sketch: ");
                log->WriteLine(sketch->Name().GetText());
            }

            /* sketch is visible: add to file */
            else
            {
                log->Write(" + Adding sketch: ");
                log->Write(sketch->Name().GetText());

                /* add sketch lines to sketches */
                sketch_geo = sketch->GetAllGeometry();
                sketches.insert(sketches.end(), sketch_geo.begin(), sketch_geo.end());

                log->Write(" ( Entities: ");
                log->Write(to_string(sketch_geo.size()));
                log->WriteLine(" )");
            }
        }
    }

    added = dxf_gen->ExportSelectionBlock()->SelectionComp()->Add(sketches);


    /* Add body to dxf export */
    for (Body *body: *(part->Bodies()))
    {
        /* build file name */
        ostringstream output_filename;
        output_filename << dxf_output_dir;
        output_filename << part->GetStringAttribute("JobNo").GetText();
        output_filename << "_";
        output_filename << part->GetStringAttribute("Mark").GetText();
        output_filename << "-";
        output_filename << body->Name().GetText();
        output_filename << ".dxf";
        
        /* Set output file name */
        dxf_gen->SetOutputFile(output_filename.str());

        /* export body */
        log->Write("Adding body: ");
        log->WriteLine(body->Name().GetText());

        for (Features::Feature *ftr: body->GetFeatures())
        {
            if (!strstr(ftr->FeatureType().GetText(), "HOLE"))            
            {
                log->Write(" + ");
                log->Write(ftr->FeatureType().GetText());
                log->Write(": ");
                log->Write(ftr->GetFeatureName().GetText());

                if (strstr(ftr->FeatureType().GetText(), "EXTRUDE"))
                {
                    try
                    {
                        log->Write(" at (");

                        Point3d feature_location = ftr->Location();
                        log->Write(to_string(feature_location.X));
                        log->Write(", ");
                        log->Write(to_string(feature_location.Y));
                        log->Write(", ");
                        log->Write(to_string(feature_location.Z));

                        log->Write(")");
                    }
                    catch(const exception &exc)
                    {
                        log->Write("[CANNOT LOCATE ORIGIN]");
                    }
                }

                log->WriteLine("");

                for (Expression *exp: ftr->GetExpressions())
                {
                    log->Write("   + Expression: ");
                    log->Write(exp->Description().GetText());
                    log->Write(" = ");
                    log->WriteLine(to_string(exp->Value()));

                }
            }
        }

        log->WriteLine(" + Faces:");
        for (Face *f: body->GetFaces())
        {
            if (f->GetEdges().size() > 4)
            {
                log->Write("   + ");
                log->Write(f->JournalIdentifier().GetText());
                log->Write(": Edge Count=");
                log->WriteLine(to_string(f->GetEdges().size()));
            }
        }

        /* add body to export */
        added = dxf_gen->ExportSelectionBlock()->SelectionComp()->Add(body);

        /* generate DXF file */
        // NXObject *generate_result = dxf_gen->Commit();
        
        /* delete added body (so that it does not export next time) */
        dxf_gen->ExportSelectionBlock()->SelectionComp()->Remove(body);
    }

    /* destroy Dxf generator */
    dxf_gen->Destroy();
    
    /* close part */
    PartCloseResponses *part_close_res = nx_session->Parts()->NewPartCloseResponses();
    part->Close(BasePart::CloseWholeTreeFalse, BasePart::CloseModifiedUseResponses, part_close_res);
    
    /* clean up part objects */
    delete part_close_res;

    nx_session->ApplicationSwitchImmediate("UG_APP_NOPART");
}

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    Session *nx_session = Session::GetSession();

    nx_session->LogFile()->WriteLine("*************************************************************");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*                     NXOpen Dxf Export                     *");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*************************************************************");
    
    run(nx_session);
    
    nx_session->LogFile()->WriteLine("*************************************************************");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*                            End                            *");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*************************************************************");

}
