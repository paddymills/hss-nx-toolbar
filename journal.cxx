// NX 1953
// Journal created by pmiller1 on Thu Feb 18 09:20:57 2021 Eastern Standard Time
//
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
// We are currently testing removal of using namespace NXOpen.
// Uncomment the below line if your journal does not compile.
// using namespace NXOpen;

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)NXOpen::Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    NXOpen::Session *theSession = NXOpen::Session::GetSession();
    NXOpen::Part *workPart(theSession->Parts()->Work());
    NXOpen::Part *displayPart(theSession->Parts()->Display());
    // ----------------------------------------------
    //   Menu: File->Export->AutoCAD DXF/DWG...
    // ----------------------------------------------
    NXOpen::Session::UndoMarkId markId1;
    markId1 = theSession->SetUndoMark(NXOpen::Session::MarkVisibilityVisible, "Start");
    
    NXOpen::DxfdwgCreator *dxfdwgCreator1;
    dxfdwgCreator1 = theSession->DexManager()->CreateDxfdwgCreator();
    
    dxfdwgCreator1->SetExportData(NXOpen::DxfdwgCreator::ExportDataOptionDrawing);
    
    dxfdwgCreator1->SetAutoCADRevision(NXOpen::DxfdwgCreator::AutoCADRevisionOptionsR2004);
    
    dxfdwgCreator1->SetViewEditMode(true);
    
    dxfdwgCreator1->SetFlattenAssembly(true);
    
    dxfdwgCreator1->SetExportScaleValue(1.0);
    
    dxfdwgCreator1->SetSettingsFile("C:\\Program Files\\Siemens\\NX1953\\dxfdwg\\dxfdwg.def");
    
    dxfdwgCreator1->SetExportAs(NXOpen::DxfdwgCreator::ExportAsOptionThreeD);
    
    dxfdwgCreator1->ExportSelectionBlock()->SetSelectionScope(NXOpen::ObjectSelector::ScopeSelectedObjects);
    
    dxfdwgCreator1->SetAutoCADRevision(NXOpen::DxfdwgCreator::AutoCADRevisionOptionsR2018);
    
    dxfdwgCreator1->SetExportFacesAs(NXOpen::DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    
    dxfdwgCreator1->SetFlattenAssembly(false);
    
    dxfdwgCreator1->SetInputFile("C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt");
    
    theSession->SetUndoMarkName(markId1, "Export AutoCAD DXF/DWG File Dialog");
    
    // ----------------------------------------------
    //   Dialog Begin Export AutoCAD DXF/DWG File
    // ----------------------------------------------
    NXOpen::Body *body1(dynamic_cast<NXOpen::Body *>(workPart->Bodies()->FindObject("SPLIT BODY(104)1")));
    bool added1;
    added1 = dxfdwgCreator1->ExportSelectionBlock()->SelectionComp()->Add(body1);
    
    NXOpen::Features::SketchFeature *sketchFeature1(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(61)")));
    NXOpen::Sketch *sketch1(dynamic_cast<NXOpen::Sketch *>(sketchFeature1->FindObject("ZINC_INTSTIFF1")));
    bool added2;
    added2 = dxfdwgCreator1->ExportSelectionBlock()->SelectionComp()->Add(sketch1);
    
    NXOpen::Session::UndoMarkId markId2;
    markId2 = theSession->SetUndoMark(NXOpen::Session::MarkVisibilityInvisible, "Export AutoCAD DXF/DWG File");
    
    theSession->DeleteUndoMark(markId2, NULL);
    
    NXOpen::Session::UndoMarkId markId3;
    markId3 = theSession->SetUndoMark(NXOpen::Session::MarkVisibilityInvisible, "Export AutoCAD DXF/DWG File");
    
    dxfdwgCreator1->SetOutputFile("T:\\SNDataPrd\\DXF\\1190181A_G1A-test.dxf");
    dxfdwgCreator1->SetSettingsFile("C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def");
    
    dxfdwgCreator1->SetWidthFactorMode(NXOpen::DxfdwgCreator::WidthfactorMethodOptionsAutomaticCalculation);
    
    // dxfdwgCreator1->SetTextFontMappingFile("C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil1C48CBA6q909.txt");
    // dxfdwgCreator1->SetCrossHatchMappingFile("C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil1C48CBA6q90a.txt");
    // dxfdwgCreator1->SetLineFontMappingFile("C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil1C48CBA6q90b.txt");
    
    dxfdwgCreator1->SetLayerMask("1-256");
    
    dxfdwgCreator1->SetDrawingList("");
    
    dxfdwgCreator1->SetViewList("");
    
    dxfdwgCreator1->SetProcessHoldFlag(true);
    
    NXOpen::NXObject *nXObject1;
    nXObject1 = dxfdwgCreator1->Commit();
    
    theSession->DeleteUndoMark(markId3, NULL);
    
    theSession->SetUndoMarkName(markId1, "Export AutoCAD DXF/DWG File");
    
    dxfdwgCreator1->Destroy();
    
    // ----------------------------------------------
    //   Menu: Tools->Journal->Stop Recording
    // ----------------------------------------------
}
