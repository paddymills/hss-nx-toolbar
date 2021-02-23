
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
    BasePart *part = nx_session->Parts()->OpenActiveDisplay(part_file_name, DisplayPartOptionReplaceExisting, &part_load_status);
    delete part_load_status;
    
    Part *work_part(nx_session->Parts()->Work());
    Part *display_part(nx_session->Parts()->Display());
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

    for (Sketch *iter: *(work_part->Sketches()))
    {

        if (strstr(iter->Name().GetText(), "ZINC"))
        {
            log->Write("Adding sketch: ");
            log->WriteLine(iter->Name().GetText());

            for (auto geo: iter->GetAllGeometry())
            {

                log->Write(" + ");
                log->WriteLine(geo->Name().GetText());

                sketches.push_back(geo);
            }
        }
    }

    added = dxf_gen->ExportSelectionBlock()->SelectionComp()->Add(sketches);


    /* Add body to dxf export */
    for (Body *iter: *(work_part->Bodies()))
    {
        /* build file name */
        ostringstream output_filename;
        output_filename << dxf_output_dir;
        output_filename << work_part->GetStringAttribute("JobNo").GetText();
        output_filename << "_";
        output_filename << work_part->GetStringAttribute("Mark").GetText();
        output_filename << "-";
        output_filename << iter->Name().GetText();
        output_filename << ".dxf";
        
        /* Set output file name */
        dxf_gen->SetOutputFile(output_filename.str());

        /* export body */
        log->Write("Adding body: ");
        log->WriteLine(iter->Name().GetText());

        /* add body to export */
        added = dxf_gen->ExportSelectionBlock()->SelectionComp()->Add(iter);

        /* generate DXF file */
        NXObject *generate_result = dxf_gen->Commit();
        
        /* delete added body (so that it does not export next time) */
        dxf_gen->ExportSelectionBlock()->SelectionComp()->Remove(iter);
    }

    /* destroy Dxf generator */
    dxf_gen->Destroy();
    
    /* close part */
    PartCloseResponses *part_close_res = nx_session->Parts()->NewPartCloseResponses();
    work_part->Close(BasePart::CloseWholeTreeFalse, BasePart::CloseModifiedUseResponses, part_close_res);
    
    /* clean up part objects */
    work_part = NULL;
    display_part = NULL;
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
