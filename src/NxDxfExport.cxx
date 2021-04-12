

#include "config.hxx"
#include "filesystem.hxx"
#include "NxDxfExport.hxx"

#include <experimental/filesystem>

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/DexBuilder.hxx>
#include <NXOpen/DexManager.hxx>
#include <NXOpen/DxfdwgCreator.hxx>

#include <NXOpen/NXObject.hxx>
#include <NXOpen/DraftingManager.hxx>
#include <NXOpen/SelectDisplayableObject.hxx>
#include <NXOpen/Annotations_Annotation.hxx>
#include <NXOpen/Annotations_AnnotationManager.hxx>
#include <NXOpen/Annotations_LeaderBuilder.hxx>
#include <NXOpen/Annotations_LeaderData.hxx>
#include <NXOpen/Annotations_LeaderDataList.hxx>
#include <NXOpen/Annotations_OriginBuilder.hxx>

using namespace NXOpen;
using namespace std;

NxDxfExport::NxDxfExport()
{
    _nxDxf = Session::GetSession()->DexManager()->CreateDxfdwgCreator();
    
    // init dxf/dwg exporter
    init_settings();
}

void NxDxfExport::init_settings()
{
    _nxDxf->SetSettingsFile(config::DXF_OUTPUT_DIR);
    _nxDxf->SetExportData(DxfdwgCreator::ExportDataOptionDrawing);
    _nxDxf->SetViewEditMode(true);
    _nxDxf->SetFlattenAssembly(true);
    _nxDxf->SetExportAs(DxfdwgCreator::ExportAsOptionThreeD);
    _nxDxf->ExportSelectionBlock()->SetSelectionScope(ObjectSelector::ScopeSelectedObjects);
    _nxDxf->SetExportFacesAs(DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    _nxDxf->SetProcessHoldFlag(true);
}

bool NxDxfExport::add_sketch(Sketch* sk)
{
    return add( sk->GetAllGeometry() );
}

bool NxDxfExport::export_body(Body* body)
{
    try
    {
        // add body to export
        body->SetLayer(config::layers::Profile);
        add(body);

        /* 
            delete part file if it exists
            this seems to speed up export compared to overwriting files
            also, it keeps from accumulating* _bk.dxf files
        */
        remove_if_exists( _nxDxf->OutputFile().GetText() );

        NXObject* generate_result = _nxDxf->Commit();
        
        // remove body from export
        remove(body);

        return true;
    }
    catch (const exception& ex) { return false; }
}
