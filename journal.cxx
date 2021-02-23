// NX 1953
// Journal created by pmiller1 on Fri Feb 19 10:59:22 2021 Eastern Standard Time
//
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
    // ----------------------------------------------
    //   Menu: File->Open...
    // ----------------------------------------------
    NXOpen::BasePart *basePart1;
    NXOpen::PartLoadStatus *partLoadStatus1;
    basePart1 = theSession->Parts()->OpenActiveDisplay("C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt", NXOpen::DisplayPartOptionAllowAdditional, &partLoadStatus1);
    
    NXOpen::Part *workPart(theSession->Parts()->Work());
    NXOpen::Part *displayPart(theSession->Parts()->Display());
    delete partLoadStatus1;
    theSession->ApplicationSwitchImmediate("UG_APP_MODELING");
    
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
    // ----------------------------------------------
    //   Dialog Begin Export AutoCAD DXF/DWG File
    // ----------------------------------------------
    NXOpen::Body *body1(dynamic_cast<NXOpen::Body *>(workPart->Bodies()->FindObject("SPLIT BODY(104)1")));
    bool added1;
    added1 = dxfdwgCreator1->ExportSelectionBlock()->SelectionComp()->Add(body1);
    
    std::vector<NXOpen::NXObject *> objects1(92);
    NXOpen::Features::SketchFeature *sketchFeature1(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(13)")));
    NXOpen::Sketch *sketch1(dynamic_cast<NXOpen::Sketch *>(sketchFeature1->FindObject("ZINC_ENDS")));
    NXOpen::Line *line1(dynamic_cast<NXOpen::Line *>(sketch1->FindObject("HANDLE R-39609")));
    objects1[0] = line1;
    NXOpen::Line *line2(dynamic_cast<NXOpen::Line *>(sketch1->FindObject("HANDLE R-39625")));
    objects1[1] = line2;
    NXOpen::Features::SketchFeature *sketchFeature2(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(28)")));
    NXOpen::Sketch *sketch2(dynamic_cast<NXOpen::Sketch *>(sketchFeature2->FindObject("ZINC_PICKPTS")));
    NXOpen::Line *line3(dynamic_cast<NXOpen::Line *>(sketch2->FindObject("HANDLE R-42349")));
    objects1[2] = line3;
    NXOpen::Line *line4(dynamic_cast<NXOpen::Line *>(sketch2->FindObject("HANDLE R-42595")));
    objects1[3] = line4;
    NXOpen::Features::SketchFeature *sketchFeature3(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(21)")));
    NXOpen::Sketch *sketch3(dynamic_cast<NXOpen::Sketch *>(sketchFeature3->FindObject("ZINC_STIFFENERS")));
    NXOpen::Line *line5(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278481")));
    objects1[4] = line5;
    NXOpen::Line *line6(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278493")));
    objects1[5] = line6;
    NXOpen::Line *line7(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278499")));
    objects1[6] = line7;
    NXOpen::Line *line8(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278511")));
    objects1[7] = line8;
    NXOpen::Features::SketchFeature *sketchFeature4(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(51)")));
    NXOpen::Sketch *sketch4(dynamic_cast<NXOpen::Sketch *>(sketchFeature4->FindObject("ZINC_BRGSTIFF1")));
    NXOpen::Line *line9(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5385439")));
    objects1[8] = line9;
    objects1[9] = sketch4;
    NXOpen::Line *line10(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5385571")));
    objects1[10] = line10;
    NXOpen::Line *line11(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5385989")));
    objects1[11] = line11;
    NXOpen::Features::SketchFeature *sketchFeature5(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(53)")));
    NXOpen::Sketch *sketch5(dynamic_cast<NXOpen::Sketch *>(sketchFeature5->FindObject("ZINC_BRGSTIFF2")));
    NXOpen::Line *line12(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5387795")));
    objects1[12] = line12;
    objects1[13] = sketch5;
    NXOpen::Line *line13(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5387927")));
    objects1[14] = line13;
    NXOpen::Line *line14(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5388345")));
    objects1[15] = line14;
    NXOpen::Features::SketchFeature *sketchFeature6(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(55)")));
    NXOpen::Sketch *sketch6(dynamic_cast<NXOpen::Sketch *>(sketchFeature6->FindObject("ZINC_BRGSTIFF3")));
    NXOpen::Line *line15(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5390150")));
    objects1[16] = line15;
    objects1[17] = sketch6;
    NXOpen::Line *line16(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5390282")));
    objects1[18] = line16;
    NXOpen::Line *line17(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5390700")));
    objects1[19] = line17;
    NXOpen::Features::SketchFeature *sketchFeature7(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(58)")));
    NXOpen::Sketch *sketch7(dynamic_cast<NXOpen::Sketch *>(sketchFeature7->FindObject("ZINC_JACKSTIFF1")));
    NXOpen::Line *line18(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5392640")));
    objects1[20] = line18;
    objects1[21] = sketch7;
    NXOpen::Line *line19(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5393058")));
    objects1[22] = line19;
    NXOpen::Features::SketchFeature *sketchFeature8(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(61)")));
    NXOpen::Sketch *sketch8(dynamic_cast<NXOpen::Sketch *>(sketchFeature8->FindObject("ZINC_INTSTIFF1")));
    NXOpen::Line *line20(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5395002")));
    objects1[23] = line20;
    objects1[24] = sketch8;
    NXOpen::Line *line21(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5395421")));
    objects1[25] = line21;
    NXOpen::Features::SketchFeature *sketchFeature9(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(20)")));
    NXOpen::Sketch *sketch9(dynamic_cast<NXOpen::Sketch *>(sketchFeature9->FindObject("REF_STIFFENERS")));
    NXOpen::Line *line22(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-112770")));
    objects1[26] = line22;
    NXOpen::Line *line23(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-112782")));
    objects1[27] = line23;
    NXOpen::Line *line24(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-243607")));
    objects1[28] = line24;
    NXOpen::Line *line25(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-244213")));
    objects1[29] = line25;
    NXOpen::Line *line26(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-244976")));
    objects1[30] = line26;
    NXOpen::Line *line27(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-245582")));
    objects1[31] = line27;
    NXOpen::Line *line28(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278452")));
    objects1[32] = line28;
    NXOpen::Line *line29(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278475")));
    objects1[33] = line29;
    NXOpen::Features::SketchFeature *sketchFeature10(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(26)")));
    NXOpen::Sketch *sketch10(dynamic_cast<NXOpen::Sketch *>(sketchFeature10->FindObject("ZINC_SETLINES")));
    NXOpen::Line *line30(dynamic_cast<NXOpen::Line *>(sketch10->FindObject("HANDLE R-413352")));
    objects1[34] = line30;
    NXOpen::Line *line31(dynamic_cast<NXOpen::Line *>(sketch10->FindObject("HANDLE R-454389")));
    objects1[35] = line31;
    NXOpen::Line *line32(dynamic_cast<NXOpen::Line *>(sketch10->FindObject("HANDLE R-454394")));
    objects1[36] = line32;
    NXOpen::Features::SketchFeature *sketchFeature11(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(25)")));
    NXOpen::Sketch *sketch11(dynamic_cast<NXOpen::Sketch *>(sketchFeature11->FindObject("REF_SETLINES")));
    NXOpen::Line *line33(dynamic_cast<NXOpen::Line *>(sketch11->FindObject("HANDLE R-2494329")));
    objects1[37] = line33;
    NXOpen::Line *line34(dynamic_cast<NXOpen::Line *>(sketch10->FindObject("HANDLE R-2753858")));
    objects1[38] = line34;
    NXOpen::Line *line35(dynamic_cast<NXOpen::Line *>(sketch10->FindObject("HANDLE R-2754178")));
    objects1[39] = line35;
    NXOpen::Line *line36(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-4723801")));
    objects1[40] = line36;
    NXOpen::Line *line37(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-4724949")));
    objects1[41] = line37;
    NXOpen::Features::SketchFeature *sketchFeature12(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(50)")));
    NXOpen::Sketch *sketch12(dynamic_cast<NXOpen::Sketch *>(sketchFeature12->FindObject("BRGSTIFF1REF")));
    NXOpen::Line *line38(dynamic_cast<NXOpen::Line *>(sketch12->FindObject("HANDLE R-5384727")));
    objects1[42] = line38;
    objects1[43] = sketch12;
    NXOpen::Line *line39(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5385778")));
    objects1[44] = line39;
    NXOpen::Line *line40(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5386140")));
    objects1[45] = line40;
    NXOpen::Features::SketchFeature *sketchFeature13(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(52)")));
    NXOpen::Sketch *sketch13(dynamic_cast<NXOpen::Sketch *>(sketchFeature13->FindObject("BRGSTIFF2REF")));
    NXOpen::Line *line41(dynamic_cast<NXOpen::Line *>(sketch13->FindObject("HANDLE R-5387083")));
    objects1[46] = line41;
    objects1[47] = sketch13;
    NXOpen::Line *line42(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5388134")));
    objects1[48] = line42;
    NXOpen::Line *line43(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5388496")));
    objects1[49] = line43;
    NXOpen::Features::SketchFeature *sketchFeature14(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(54)")));
    NXOpen::Sketch *sketch14(dynamic_cast<NXOpen::Sketch *>(sketchFeature14->FindObject("BRGSTIFF3REF")));
    NXOpen::Line *line44(dynamic_cast<NXOpen::Line *>(sketch14->FindObject("HANDLE R-5389438")));
    objects1[50] = line44;
    objects1[51] = sketch14;
    NXOpen::Line *line45(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5390489")));
    objects1[52] = line45;
    NXOpen::Line *line46(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5390851")));
    objects1[53] = line46;
    NXOpen::Features::SketchFeature *sketchFeature15(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(57)")));
    NXOpen::Sketch *sketch15(dynamic_cast<NXOpen::Sketch *>(sketchFeature15->FindObject("JACKSTIFF1REF")));
    NXOpen::Line *line47(dynamic_cast<NXOpen::Line *>(sketch15->FindObject("HANDLE R-5391794")));
    objects1[54] = line47;
    objects1[55] = sketch15;
    NXOpen::Line *line48(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5392508")));
    objects1[56] = line48;
    NXOpen::Line *line49(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5392847")));
    objects1[57] = line49;
    NXOpen::Line *line50(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5393209")));
    objects1[58] = line50;
    NXOpen::Features::SketchFeature *sketchFeature16(dynamic_cast<NXOpen::Features::SketchFeature *>(workPart->Features()->FindObject("SKETCH(60)")));
    NXOpen::Sketch *sketch16(dynamic_cast<NXOpen::Sketch *>(sketchFeature16->FindObject("INTSTIFF1REF")));
    NXOpen::Line *line51(dynamic_cast<NXOpen::Line *>(sketch16->FindObject("HANDLE R-5394152")));
    objects1[59] = line51;
    objects1[60] = sketch16;
    NXOpen::Line *line52(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5394870")));
    objects1[61] = line52;
    NXOpen::Line *line53(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5395209")));
    objects1[62] = line53;
    NXOpen::Line *line54(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5395572")));
    objects1[63] = line54;
    NXOpen::Line *line55(dynamic_cast<NXOpen::Line *>(sketch1->FindObject("HANDLE R-39740")));
    objects1[64] = line55;
    NXOpen::Line *line56(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-112764")));
    objects1[65] = line56;
    NXOpen::Line *line57(dynamic_cast<NXOpen::Line *>(sketch9->FindObject("HANDLE R-112776")));
    objects1[66] = line57;
    NXOpen::Line *line58(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278463")));
    objects1[67] = line58;
    NXOpen::Line *line59(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278469")));
    objects1[68] = line59;
    NXOpen::Line *line60(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278487")));
    objects1[69] = line60;
    NXOpen::Line *line61(dynamic_cast<NXOpen::Line *>(sketch3->FindObject("HANDLE R-278505")));
    objects1[70] = line61;
    NXOpen::Line *line62(dynamic_cast<NXOpen::Line *>(sketch11->FindObject("HANDLE R-393649")));
    objects1[71] = line62;
    NXOpen::Line *line63(dynamic_cast<NXOpen::Line *>(sketch12->FindObject("HANDLE R-5384513")));
    objects1[72] = line63;
    NXOpen::Line *line64(dynamic_cast<NXOpen::Line *>(sketch12->FindObject("HANDLE R-5384792")));
    objects1[73] = line64;
    NXOpen::Line *line65(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5384962")));
    objects1[74] = line65;
    NXOpen::Line *line66(dynamic_cast<NXOpen::Line *>(sketch4->FindObject("HANDLE R-5385210")));
    objects1[75] = line66;
    NXOpen::Line *line67(dynamic_cast<NXOpen::Line *>(sketch13->FindObject("HANDLE R-5386869")));
    objects1[76] = line67;
    NXOpen::Line *line68(dynamic_cast<NXOpen::Line *>(sketch13->FindObject("HANDLE R-5387148")));
    objects1[77] = line68;
    NXOpen::Line *line69(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5387318")));
    objects1[78] = line69;
    NXOpen::Line *line70(dynamic_cast<NXOpen::Line *>(sketch5->FindObject("HANDLE R-5387566")));
    objects1[79] = line70;
    NXOpen::Line *line71(dynamic_cast<NXOpen::Line *>(sketch14->FindObject("HANDLE R-5389224")));
    objects1[80] = line71;
    NXOpen::Line *line72(dynamic_cast<NXOpen::Line *>(sketch14->FindObject("HANDLE R-5389503")));
    objects1[81] = line72;
    NXOpen::Line *line73(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5389673")));
    objects1[82] = line73;
    NXOpen::Line *line74(dynamic_cast<NXOpen::Line *>(sketch6->FindObject("HANDLE R-5389921")));
    objects1[83] = line74;
    NXOpen::Line *line75(dynamic_cast<NXOpen::Line *>(sketch15->FindObject("HANDLE R-5391580")));
    objects1[84] = line75;
    NXOpen::Line *line76(dynamic_cast<NXOpen::Line *>(sketch15->FindObject("HANDLE R-5391859")));
    objects1[85] = line76;
    NXOpen::Line *line77(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5392031")));
    objects1[86] = line77;
    NXOpen::Line *line78(dynamic_cast<NXOpen::Line *>(sketch7->FindObject("HANDLE R-5392279")));
    objects1[87] = line78;
    NXOpen::Line *line79(dynamic_cast<NXOpen::Line *>(sketch16->FindObject("HANDLE R-5393937")));
    objects1[88] = line79;
    NXOpen::Line *line80(dynamic_cast<NXOpen::Line *>(sketch16->FindObject("HANDLE R-5394217")));
    objects1[89] = line80;
    NXOpen::Line *line81(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5394393")));
    objects1[90] = line81;
    NXOpen::Line *line82(dynamic_cast<NXOpen::Line *>(sketch8->FindObject("HANDLE R-5394641")));
    objects1[91] = line82;
    bool added2;
    added2 = dxfdwgCreator1->ExportSelectionBlock()->SelectionComp()->Add(objects1);
    
    NXOpen::Session::UndoMarkId markId2;
    markId2 = theSession->SetUndoMark(NXOpen::Session::MarkVisibilityInvisible, "Export AutoCAD DXF/DWG File");
    
    theSession->DeleteUndoMark(markId2, NULL);
    
    NXOpen::Session::UndoMarkId markId3;
    markId3 = theSession->SetUndoMark(NXOpen::Session::MarkVisibilityInvisible, "Export AutoCAD DXF/DWG File");
    
    dxfdwgCreator1->SetOutputFile("T:\\SNDataPrd\\DXF\\1190181A_G1A-web_named_bodies.dxf");
    
    dxfdwgCreator1->SetTextFontMappingFile("C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil4BE06903s9l7.txt");
    
    dxfdwgCreator1->SetWidthFactorMode(NXOpen::DxfdwgCreator::WidthfactorMethodOptionsAutomaticCalculation);
    
    dxfdwgCreator1->SetCrossHatchMappingFile("C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil4BE06903s9l8.txt");
    
    dxfdwgCreator1->SetLineFontMappingFile("C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil4BE06903s9l9.txt");
    
    dxfdwgCreator1->SetLayerMask("1-256");
    
    dxfdwgCreator1->SetDrawingList("");
    
    dxfdwgCreator1->SetViewList("");
    
    dxfdwgCreator1->SetProcessHoldFlag(true);
    
    NXOpen::NXObject *nXObject1;
    nXObject1 = dxfdwgCreator1->Commit();
    
    theSession->DeleteUndoMark(markId3, NULL);
    
    theSession->SetUndoMarkName(markId1, "Export AutoCAD DXF/DWG File");
    
    dxfdwgCreator1->Destroy();
    
    NXOpen::PartCloseResponses *partCloseResponses1;
    partCloseResponses1 = theSession->Parts()->NewPartCloseResponses();
    
    workPart->Close(NXOpen::BasePart::CloseWholeTreeFalse, NXOpen::BasePart::CloseModifiedUseResponses, partCloseResponses1);
    
    workPart = NULL;
    displayPart = NULL;
    delete partCloseResponses1;
    theSession->ApplicationSwitchImmediate("UG_APP_NOPART");
    
    // ----------------------------------------------
    //   Menu: Tools->Journal->Stop Recording
    // ----------------------------------------------
}
