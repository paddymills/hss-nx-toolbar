
#include <iostream>

#include <uf_defs.h>
#include <NXOpen/NXException.hxx>
#include <NXOpen/Session.hxx>
#include <NXOpen/BaseCreator.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Builder.hxx>
#include <NXOpen/DexBuilder.hxx>
#include <NXOpen/DexManager.hxx>
#include <NXOpen/DxfdwgCreator.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_SketchFeature.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/ObjectSelector.hxx>
#include <NXOpen/ObjectTypeSelector.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/PreviewBuilder.hxx>
#include <NXOpen/SelectNXObjectList.hxx>
#include <NXOpen/SelectObjectList.hxx>
#include <NXOpen/Session.hxx>
#include <NXOpen/Sketch.hxx>

#include <NXOpen/PartLoadStatus.hxx>
#include <NXOpen/LicenseManager.hxx>

using namespace NXOpen;
using namespace std;

int main(int argc, char* argv[])
{
    PartLoadStatus *loadStatus;
    char *filename = "C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt";
    
    char *contextName = "DxfExport";
    BasePart::CloseWholeTree closeTree = BasePart::CloseWholeTreeTrue;
    BasePart::CloseModified closeModified = BasePart::CloseModifiedCloseModified;

    Session *theSession = Session::GetSession();
    /* acquire license */
    theSession->LicenseManager()->Reserve("solid_modeling", contextName);

    Part *part = theSession->Parts()->Open(filename, &loadStatus);
    NXOpen::Part *workPart(theSession->Parts()->Work());
    NXOpen::Part *displayPart(theSession->Parts()->Display());

    // ----------------------------------------------
    //   Menu: File->Export->AutoCAD DXF/DWG...
    // ----------------------------------------------
    
    DxfdwgCreator *dxfdwgCreator;
    dxfdwgCreator = theSession->DexManager()->CreateDxfdwgCreator();
    
    dxfdwgCreator->SetExportData(DxfdwgCreator::ExportDataOptionDrawing);
    dxfdwgCreator->SetAutoCADRevision(DxfdwgCreator::AutoCADRevisionOptionsR2018);
    dxfdwgCreator->SetViewEditMode(true);
    dxfdwgCreator->SetFlattenAssembly(true);
    dxfdwgCreator->SetExportScaleValue(1.0);
    dxfdwgCreator->SetSettingsFile("C:\\Program Files\\Siemens\\NX1953\\dxfdwg\\dxfdwg.def");
    dxfdwgCreator->SetExportAs(DxfdwgCreator::ExportAsOptionThreeD);
    dxfdwgCreator->ExportSelectionBlock()->SetSelectionScope(ObjectSelector::ScopeSelectedObjects);
    dxfdwgCreator->SetExportFacesAs(DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    dxfdwgCreator->SetFlattenAssembly(false);
    dxfdwgCreator->SetInputFile(filename);

    dxfdwgCreator->SetOutputFile("T:\\SNDataPrd\\DXF\\1190181A_G1A-test.dxf");
    dxfdwgCreator->SetSettingsFile("C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def");
    dxfdwgCreator->SetWidthFactorMode(NXOpen::DxfdwgCreator::WidthfactorMethodOptionsAutomaticCalculation);
    dxfdwgCreator->SetLayerMask("1-256");
    dxfdwgCreator->SetDrawingList("");
    dxfdwgCreator->SetViewList("");
    dxfdwgCreator->SetProcessHoldFlag(true);
    
    // ----------------------------------------------
    //   Dialog Begin Export AutoCAD DXF/DWG File
    // ----------------------------------------------
    
    bool addedSelection;
    NXObject *commitResult;

    BodyCollection *bodies = part->Bodies();
    BodyCollection::iterator iter;
    Body *body;

    for (iter = bodies->begin(); iter != bodies->end(); iter++)
    {
        body = *iter;
        addedSelection = dxfdwgCreator->ExportSelectionBlock()->SelectionComp()->Add(body);

        break;
    }
    
    // Features::SketchFeature *sketchFeature1(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(61)")));
    // Sketch *sketch1(dynamic_cast<Sketch *>(sketchFeature1->FindObject("ZINC_INTSTIFF1")));
    // bool added2;
    // added2 = dxfdwgCreator->ExportSelectionBlock()->SelectionComp()->Add(sketch1);
    
    commitResult = dxfdwgCreator->Commit();

    dxfdwgCreator->Destroy();

    /* Close the part */
    part->Close(closeTree, closeModified, nullptr);
    
    /* release license */
    theSession->LicenseManager()->ReleaseAll(contextName);

    return 0;
}
