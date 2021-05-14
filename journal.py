# NX 1953
# Journal created by pmiller1 on Tue May 11 11:16:10 2021 Eastern Daylight Time
#
import math
import NXOpen
import NXOpen.Annotations
import NXOpen.Drawings
import NXOpen.Features
def main() : 

    theSession  = NXOpen.Session.GetSession()
    markId1 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Load Part")
    
    basePart1, partLoadStatus1 = theSession.Parts.OpenActiveDisplay("C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\1190181A_G1A-web_named.prt", NXOpen.DisplayPartOption.AllowAdditional)
    
    workPart = theSession.Parts.Work
    displayPart = theSession.Parts.Display
    partLoadStatus1.Dispose()
    theSession.ApplicationSwitchImmediate("UG_APP_MODELING")
    
    # ----------------------------------------------
    #   Menu: Application->Document->Drafting
    # ----------------------------------------------
    markId2 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Enter Drafting")
    
    theSession.ApplicationSwitchImmediate("UG_APP_DRAFTING")
    
    workPart.Drafting.EnterDraftingApplication()
    
    workPart.Views.WorkView.UpdateCustomSymbols()
    
    workPart.Drafting.SetTemplateInstantiationIsComplete(True)
    
    # ----------------------------------------------
    #   Menu: View->Display Sheet
    # ----------------------------------------------
    workPart.Drafting.SetDrawingLayout(False)
    
    # ----------------------------------------------
    #   Menu: Insert->Annotation->Note...
    # ----------------------------------------------
    markId3 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Start")
    
    draftingNoteBuilder1 = workPart.Annotations.CreateDraftingNoteBuilder(NXOpen.Annotations.SimpleDraftingAid.Null)
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder1.Origin.Anchor = NXOpen.Annotations.OriginBuilder.AlignmentPosition.BottomRight
    
    text1 = [None] * 2 
    text1[0] = "DRAWING: 1AB"
    text1[1] = "THICKNESS: 0.875"
    draftingNoteBuilder1.Text.TextBlock.SetText(text1)
    
    theSession.SetUndoMarkName(markId3, "Note Dialog")
    
    draftingNoteBuilder1.Origin.Plane.PlaneMethod = NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    leaderData1 = workPart.Annotations.CreateLeaderData()
    
    leaderData1.Arrowhead = NXOpen.Annotations.LeaderData.ArrowheadType.FilledArrow
    
    leaderData1.VerticalAttachment = NXOpen.Annotations.LeaderVerticalAttachment.Center
    
    draftingNoteBuilder1.Leader.Leaders.Append(leaderData1)
    
    leaderData1.StubSide = NXOpen.Annotations.LeaderSide.Inferred
    
    symbolscale1 = draftingNoteBuilder1.Text.TextBlock.SymbolScale
    
    symbolaspectratio1 = draftingNoteBuilder1.Text.TextBlock.SymbolAspectRatio
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    markId4 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Start")
    
    theSession.SetUndoMarkName(markId4, "Note Settings Dialog")
    
    theSession.SetUndoMarkVisibility(markId4, None, NXOpen.Session.MarkVisibility.Visible)
    
    # ----------------------------------------------
    #   Dialog Begin Note Settings
    # ----------------------------------------------
    markId5 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Note Settings")
    
    theSession.DeleteUndoMark(markId5, None)
    
    markId6 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Note Settings")
    
    draftingNoteBuilder1.Style.LetteringStyle.GeneralTextSize = 5.0
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    theSession.SetUndoMarkName(markId6, "Note Settings - Height")
    
    theSession.SetUndoMarkVisibility(markId6, None, NXOpen.Session.MarkVisibility.Visible)
    
    theSession.SetUndoMarkVisibility(markId4, None, NXOpen.Session.MarkVisibility.Invisible)
    
    markId7 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Note Settings")
    
    markId8 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Note Settings")
    
    theSession.DeleteUndoMark(markId8, None)
    
    theSession.SetUndoMarkName(markId4, "Note Settings")
    
    theSession.DeleteUndoMark(markId7, None)
    
    theSession.SetUndoMarkVisibility(markId4, None, NXOpen.Session.MarkVisibility.Visible)
    
    theSession.DeleteUndoMark(markId6, None)
    
    theSession.DeleteUndoMark(markId4, None)
    
    assocOrigin1 = NXOpen.Annotations.Annotation.AssociativeOriginData()
    
    assocOrigin1.OriginType = NXOpen.Annotations.AssociativeOriginType.Drag
    assocOrigin1.View = NXOpen.View.Null
    assocOrigin1.ViewOfGeometry = NXOpen.View.Null
    assocOrigin1.PointOnGeometry = NXOpen.Point.Null
    assocOrigin1.VertAnnotation = NXOpen.Annotations.Annotation.Null
    assocOrigin1.VertAlignmentPosition = NXOpen.Annotations.AlignmentPosition.TopLeft
    assocOrigin1.HorizAnnotation = NXOpen.Annotations.Annotation.Null
    assocOrigin1.HorizAlignmentPosition = NXOpen.Annotations.AlignmentPosition.TopLeft
    assocOrigin1.AlignedAnnotation = NXOpen.Annotations.Annotation.Null
    assocOrigin1.DimensionLine = 0
    assocOrigin1.AssociatedView = NXOpen.View.Null
    assocOrigin1.AssociatedPoint = NXOpen.Point.Null
    assocOrigin1.OffsetAnnotation = NXOpen.Annotations.Annotation.Null
    assocOrigin1.OffsetAlignmentPosition = NXOpen.Annotations.AlignmentPosition.TopLeft
    assocOrigin1.XOffsetFactor = 0.0
    assocOrigin1.YOffsetFactor = 0.0
    assocOrigin1.StackAlignmentPosition = NXOpen.Annotations.StackAlignmentPosition.Above
    draftingNoteBuilder1.Origin.SetAssociativeOrigin(assocOrigin1)
    
    point1 = NXOpen.Point3d(126.71653856128955, -28.261518450521606, 0.0)
    draftingNoteBuilder1.Origin.Origin.SetValue(NXOpen.TaggedObject.Null, NXOpen.View.Null, point1)
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    markId9 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Note")
    
    nXObject1 = draftingNoteBuilder1.Commit()
    
    theSession.DeleteUndoMark(markId9, None)
    
    draftingNoteBuilder1.Origin.SetInferRelativeToGeometry(True)
    
    theSession.SetUndoMarkName(markId3, "Note")
    
    theSession.SetUndoMarkVisibility(markId3, None, NXOpen.Session.MarkVisibility.Visible)
    
    draftingNoteBuilder1.Destroy()
    
    markId10 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Start")
    
    draftingNoteBuilder2 = workPart.Annotations.CreateDraftingNoteBuilder(NXOpen.Annotations.SimpleDraftingAid.Null)
    
    draftingNoteBuilder2.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder2.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder2.Origin.Anchor = NXOpen.Annotations.OriginBuilder.AlignmentPosition.BottomRight
    
    text2 = [None] * 2 
    text2[0] = "DRAWING: 1AB"
    text2[1] = "THICKNESS: 0.875"
    draftingNoteBuilder2.Text.TextBlock.SetText(text2)
    
    draftingNoteBuilder2.Style.DimensionStyle.LimitFitDeviation = "H"
    
    draftingNoteBuilder2.Style.DimensionStyle.LimitFitShaftDeviation = "g"
    
    draftingNoteBuilder2.Style.LetteringStyle.GeneralTextSize = 5.0
    
    theSession.SetUndoMarkName(markId10, "Note Dialog")
    
    draftingNoteBuilder2.Origin.Plane.PlaneMethod = NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane
    
    draftingNoteBuilder2.Origin.SetInferRelativeToGeometry(True)
    
    leaderData2 = workPart.Annotations.CreateLeaderData()
    
    leaderData2.Arrowhead = NXOpen.Annotations.LeaderData.ArrowheadType.FilledArrow
    
    leaderData2.VerticalAttachment = NXOpen.Annotations.LeaderVerticalAttachment.Center
    
    draftingNoteBuilder2.Leader.Leaders.Append(leaderData2)
    
    leaderData2.StubSide = NXOpen.Annotations.LeaderSide.Inferred
    
    symbolscale2 = draftingNoteBuilder2.Text.TextBlock.SymbolScale
    
    symbolaspectratio2 = draftingNoteBuilder2.Text.TextBlock.SymbolAspectRatio
    
    draftingNoteBuilder2.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder2.Origin.SetInferRelativeToGeometry(True)
    
    draftingNoteBuilder2.Destroy()
    
    theSession.UndoToMark(markId10, None)
    
    theSession.DeleteUndoMark(markId10, None)
    
    # ----------------------------------------------
    #   Menu: File->Export->AutoCAD DXF/DWG...
    # ----------------------------------------------
    markId11 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Start")
    
    dxfdwgCreator1 = theSession.DexManager.CreateDxfdwgCreator()
    
    dxfdwgCreator1.ExportData = NXOpen.DxfdwgCreator.ExportDataOption.Drawing
    
    dxfdwgCreator1.AutoCADRevision = NXOpen.DxfdwgCreator.AutoCADRevisionOptions.R2004
    
    dxfdwgCreator1.ViewEditMode = True
    
    dxfdwgCreator1.FlattenAssembly = True
    
    dxfdwgCreator1.ExportScaleValue = 1.0
    
    dxfdwgCreator1.SettingsFile = "C:\\Program Files\\Siemens\\NX1953\\dxfdwg\\dxfdwg.def"
    
    dxfdwgCreator1.ExportAs = NXOpen.DxfdwgCreator.ExportAsOption.ThreeD
    
    dxfdwgCreator1.ExportSelectionBlock.SelectionScope = NXOpen.ObjectSelector.Scope.SelectedObjects
    
    dxfdwgCreator1.AutoCADRevision = NXOpen.DxfdwgCreator.AutoCADRevisionOptions.R2018
    
    dxfdwgCreator1.ExportFacesAs = NXOpen.DxfdwgCreator.ExportFacesAsOptions.PolylineMesh
    
    dxfdwgCreator1.FlattenAssembly = False
    
    dxfdwgCreator1.InputFile = "C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\1190181A_G1A-web_named.prt"
    
    theSession.SetUndoMarkName(markId11, "Export AutoCAD DXF/DWG File Dialog")
    
    # ----------------------------------------------
    #   Dialog Begin Export AutoCAD DXF/DWG File
    # ----------------------------------------------
    body1 = workPart.Bodies.FindObject("SPLIT BODY(104)1")
    added1 = dxfdwgCreator1.ExportSelectionBlock.SelectionComp.Add(body1)
    
    objects1 = [NXOpen.NXObject.Null] * 92 
    sketchFeature1 = workPart.Features.FindObject("SKETCH(13)")
    sketch1 = sketchFeature1.FindObject("ZINC_ENDS")
    line1 = sketch1.FindObject("HANDLE R-39609")
    objects1[0] = line1
    sketchFeature2 = workPart.Features.FindObject("SKETCH(53)")
    sketch2 = sketchFeature2.FindObject("ZINC_BRGSTIFF2")
    line2 = sketch2.FindObject("HANDLE R-5388345")
    objects1[1] = line2
    objects1[2] = sketch2
    line3 = sketch2.FindObject("HANDLE R-5387927")
    objects1[3] = line3
    sketchFeature3 = workPart.Features.FindObject("SKETCH(28)")
    sketch3 = sketchFeature3.FindObject("ZINC_PICKPTS")
    line4 = sketch3.FindObject("HANDLE R-42595")
    objects1[4] = line4
    sketchFeature4 = workPart.Features.FindObject("SKETCH(21)")
    sketch4 = sketchFeature4.FindObject("ZINC_STIFFENERS")
    line5 = sketch4.FindObject("HANDLE R-278499")
    objects1[5] = line5
    sketchFeature5 = workPart.Features.FindObject("SKETCH(51)")
    sketch5 = sketchFeature5.FindObject("ZINC_BRGSTIFF1")
    line6 = sketch5.FindObject("HANDLE R-5385571")
    objects1[6] = line6
    objects1[7] = sketch5
    line7 = sketch3.FindObject("HANDLE R-42349")
    objects1[8] = line7
    line8 = sketch1.FindObject("HANDLE R-39625")
    objects1[9] = line8
    sketchFeature6 = workPart.Features.FindObject("SKETCH(61)")
    sketch6 = sketchFeature6.FindObject("ZINC_INTSTIFF1")
    line9 = sketch6.FindObject("HANDLE R-5395002")
    objects1[10] = line9
    objects1[11] = sketch6
    line10 = sketch4.FindObject("HANDLE R-278463")
    objects1[12] = line10
    line11 = sketch5.FindObject("HANDLE R-5385989")
    objects1[13] = line11
    line12 = sketch4.FindObject("HANDLE R-278511")
    objects1[14] = line12
    line13 = sketch5.FindObject("HANDLE R-5385439")
    objects1[15] = line13
    line14 = sketch4.FindObject("HANDLE R-278469")
    objects1[16] = line14
    line15 = sketch4.FindObject("HANDLE R-278481")
    objects1[17] = line15
    line16 = sketch2.FindObject("HANDLE R-5387795")
    objects1[18] = line16
    sketchFeature7 = workPart.Features.FindObject("SKETCH(55)")
    sketch7 = sketchFeature7.FindObject("ZINC_BRGSTIFF3")
    line17 = sketch7.FindObject("HANDLE R-5390150")
    objects1[19] = line17
    objects1[20] = sketch7
    sketchFeature8 = workPart.Features.FindObject("SKETCH(58)")
    sketch8 = sketchFeature8.FindObject("ZINC_JACKSTIFF1")
    line18 = sketch8.FindObject("HANDLE R-5393058")
    objects1[21] = line18
    objects1[22] = sketch8
    line19 = sketch6.FindObject("HANDLE R-5395421")
    objects1[23] = line19
    line20 = sketch4.FindObject("HANDLE R-278493")
    objects1[24] = line20
    line21 = sketch8.FindObject("HANDLE R-5392640")
    objects1[25] = line21
    line22 = sketch7.FindObject("HANDLE R-5390700")
    objects1[26] = line22
    line23 = sketch7.FindObject("HANDLE R-5390282")
    objects1[27] = line23
    line24 = sketch8.FindObject("HANDLE R-5392508")
    objects1[28] = line24
    line25 = sketch4.FindObject("HANDLE R-4723801")
    objects1[29] = line25
    line26 = sketch7.FindObject("HANDLE R-5390489")
    objects1[30] = line26
    line27 = sketch5.FindObject("HANDLE R-5385778")
    objects1[31] = line27
    line28 = sketch8.FindObject("HANDLE R-5392847")
    objects1[32] = line28
    sketchFeature9 = workPart.Features.FindObject("SKETCH(26)")
    sketch9 = sketchFeature9.FindObject("ZINC_SETLINES")
    line29 = sketch9.FindObject("HANDLE R-413352")
    objects1[33] = line29
    sketchFeature10 = workPart.Features.FindObject("SKETCH(52)")
    sketch10 = sketchFeature10.FindObject("BRGSTIFF2REF")
    line30 = sketch10.FindObject("HANDLE R-5387083")
    objects1[34] = line30
    objects1[35] = sketch10
    sketchFeature11 = workPart.Features.FindObject("SKETCH(20)")
    sketch11 = sketchFeature11.FindObject("REF_STIFFENERS")
    line31 = sketch11.FindObject("HANDLE R-244213")
    objects1[36] = line31
    line32 = sketch6.FindObject("HANDLE R-5395209")
    objects1[37] = line32
    line33 = sketch9.FindObject("HANDLE R-454394")
    objects1[38] = line33
    line34 = sketch4.FindObject("HANDLE R-4724949")
    objects1[39] = line34
    line35 = sketch11.FindObject("HANDLE R-112770")
    objects1[40] = line35
    line36 = sketch6.FindObject("HANDLE R-5394870")
    objects1[41] = line36
    line37 = sketch8.FindObject("HANDLE R-5393209")
    objects1[42] = line37
    line38 = sketch7.FindObject("HANDLE R-5390851")
    objects1[43] = line38
    line39 = sketch9.FindObject("HANDLE R-2754178")
    objects1[44] = line39
    line40 = sketch11.FindObject("HANDLE R-244976")
    objects1[45] = line40
    sketchFeature12 = workPart.Features.FindObject("SKETCH(25)")
    sketch12 = sketchFeature12.FindObject("REF_SETLINES")
    line41 = sketch12.FindObject("HANDLE R-2494329")
    objects1[46] = line41
    sketchFeature13 = workPart.Features.FindObject("SKETCH(50)")
    sketch13 = sketchFeature13.FindObject("BRGSTIFF1REF")
    line42 = sketch13.FindObject("HANDLE R-5384727")
    objects1[47] = line42
    objects1[48] = sketch13
    line43 = sketch2.FindObject("HANDLE R-5388134")
    objects1[49] = line43
    sketchFeature14 = workPart.Features.FindObject("SKETCH(57)")
    sketch14 = sketchFeature14.FindObject("JACKSTIFF1REF")
    line44 = sketch14.FindObject("HANDLE R-5391794")
    objects1[50] = line44
    objects1[51] = sketch14
    line45 = sketch9.FindObject("HANDLE R-2753858")
    objects1[52] = line45
    sketchFeature15 = workPart.Features.FindObject("SKETCH(60)")
    sketch15 = sketchFeature15.FindObject("INTSTIFF1REF")
    line46 = sketch15.FindObject("HANDLE R-5394152")
    objects1[53] = line46
    objects1[54] = sketch15
    line47 = sketch6.FindObject("HANDLE R-5395572")
    objects1[55] = line47
    line48 = sketch5.FindObject("HANDLE R-5386140")
    objects1[56] = line48
    line49 = sketch9.FindObject("HANDLE R-454389")
    objects1[57] = line49
    line50 = sketch11.FindObject("HANDLE R-112782")
    objects1[58] = line50
    line51 = sketch11.FindObject("HANDLE R-245582")
    objects1[59] = line51
    line52 = sketch11.FindObject("HANDLE R-243607")
    objects1[60] = line52
    line53 = sketch2.FindObject("HANDLE R-5388496")
    objects1[61] = line53
    sketchFeature16 = workPart.Features.FindObject("SKETCH(54)")
    sketch16 = sketchFeature16.FindObject("BRGSTIFF3REF")
    line54 = sketch16.FindObject("HANDLE R-5389438")
    objects1[62] = line54
    objects1[63] = sketch16
    line55 = sketch13.FindObject("HANDLE R-5384792")
    objects1[64] = line55
    line56 = sketch4.FindObject("HANDLE R-278505")
    objects1[65] = line56
    line57 = sketch10.FindObject("HANDLE R-5386869")
    objects1[66] = line57
    line58 = sketch11.FindObject("HANDLE R-112776")
    objects1[67] = line58
    line59 = sketch2.FindObject("HANDLE R-5387566")
    objects1[68] = line59
    line60 = sketch4.FindObject("HANDLE R-278487")
    objects1[69] = line60
    line61 = sketch7.FindObject("HANDLE R-5389673")
    objects1[70] = line61
    line62 = sketch6.FindObject("HANDLE R-5394393")
    objects1[71] = line62
    line63 = sketch5.FindObject("HANDLE R-5385210")
    objects1[72] = line63
    line64 = sketch5.FindObject("HANDLE R-5384962")
    objects1[73] = line64
    line65 = sketch15.FindObject("HANDLE R-5393937")
    objects1[74] = line65
    line66 = sketch6.FindObject("HANDLE R-5394641")
    objects1[75] = line66
    line67 = sketch8.FindObject("HANDLE R-5392279")
    objects1[76] = line67
    line68 = sketch16.FindObject("HANDLE R-5389224")
    objects1[77] = line68
    line69 = sketch12.FindObject("HANDLE R-393649")
    objects1[78] = line69
    line70 = sketch1.FindObject("HANDLE R-39740")
    objects1[79] = line70
    line71 = sketch4.FindObject("HANDLE R-278452")
    objects1[80] = line71
    line72 = sketch7.FindObject("HANDLE R-5389921")
    objects1[81] = line72
    line73 = sketch13.FindObject("HANDLE R-5384513")
    objects1[82] = line73
    line74 = sketch8.FindObject("HANDLE R-5392031")
    objects1[83] = line74
    line75 = sketch11.FindObject("HANDLE R-112764")
    objects1[84] = line75
    line76 = sketch15.FindObject("HANDLE R-5394217")
    objects1[85] = line76
    line77 = sketch2.FindObject("HANDLE R-5387318")
    objects1[86] = line77
    line78 = sketch14.FindObject("HANDLE R-5391859")
    objects1[87] = line78
    line79 = sketch16.FindObject("HANDLE R-5389503")
    objects1[88] = line79
    line80 = sketch14.FindObject("HANDLE R-5391580")
    objects1[89] = line80
    line81 = sketch10.FindObject("HANDLE R-5387148")
    objects1[90] = line81
    line82 = sketch4.FindObject("HANDLE R-278475")
    objects1[91] = line82
    added2 = dxfdwgCreator1.ExportSelectionBlock.SelectionComp.Add(objects1)
    
    # ----------------------------------------------
    #   Dialog Begin Export AutoCAD DXF/DWG File
    # ----------------------------------------------
    # ----------------------------------------------
    #   Dialog Begin Export AutoCAD DXF/DWG File
    # ----------------------------------------------
    # ----------------------------------------------
    #   Dialog Begin Export AutoCAD DXF/DWG File
    # ----------------------------------------------
    # ----------------------------------------------
    #   Dialog Begin Export AutoCAD DXF/DWG File
    # ----------------------------------------------
    # ----------------------------------------------
    #   Dialog Begin Export AutoCAD DXF/DWG File
    # ----------------------------------------------
    markId12 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Export AutoCAD DXF/DWG File")
    
    theSession.DeleteUndoMark(markId12, None)
    
    markId13 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Export AutoCAD DXF/DWG File")
    
    dxfdwgCreator1.OutputFile = "T:\\SNDataPrd\\DXF\\1190181A_G1A-web_named.dxf"
    
    dxfdwgCreator1.TextFontMappingFile = "C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil2060FE20y7ap.txt"
    
    dxfdwgCreator1.WidthFactorMode = NXOpen.DxfdwgCreator.WidthfactorMethodOptions.AutomaticCalculation
    
    dxfdwgCreator1.CrossHatchMappingFile = "C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil2060FE20y7aq.txt"
    
    dxfdwgCreator1.LineFontMappingFile = "C:\\Users\\PMiller1\\AppData\\Local\\Temp\\pmil2060FE20y7ar.txt"
    
    dxfdwgCreator1.LayerMask = "1-256"
    
    dxfdwgCreator1.DrawingList = ""
    
    dxfdwgCreator1.ViewList = ""
    
    dxfdwgCreator1.ProcessHoldFlag = True
    
    filename1 = dxfdwgCreator1.InputFile
    
    nXObject2 = dxfdwgCreator1.Commit()
    
    theSession.DeleteUndoMark(markId13, None)
    
    theSession.SetUndoMarkName(markId11, "Export AutoCAD DXF/DWG File")
    
    dxfdwgCreator1.Destroy()
    
    workPart.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.UseResponses, None)
    
    workPart = NXOpen.Part.Null
    displayPart = NXOpen.Part.Null
    theSession.ApplicationSwitchImmediate("UG_APP_NOPART")
    
    # ----------------------------------------------
    #   Menu: Tools->Journal->Stop Recording
    # ----------------------------------------------
    
if __name__ == '__main__':
    main()