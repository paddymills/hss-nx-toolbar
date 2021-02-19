' NX 1953
' Journal created by pmiller1 on Thu Feb 18 08:52:40 2021 Eastern Standard Time
'
Imports System
Imports NXOpen

Module NXJournal
Sub Main (ByVal args() As String) 

Dim theSession As NXOpen.Session = NXOpen.Session.GetSession()
Dim workPart As NXOpen.Part = theSession.Parts.Work

Dim displayPart As NXOpen.Part = theSession.Parts.Display

' ----------------------------------------------
'   Menu: File->Export->AutoCAD DXF/DWG...
' ----------------------------------------------
Dim markId1 As NXOpen.Session.UndoMarkId = Nothing
markId1 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Start")

Dim dxfdwgCreator1 As NXOpen.DxfdwgCreator = Nothing
dxfdwgCreator1 = theSession.DexManager.CreateDxfdwgCreator()

dxfdwgCreator1.ExportData = NXOpen.DxfdwgCreator.ExportDataOption.Drawing

dxfdwgCreator1.AutoCADRevision = NXOpen.DxfdwgCreator.AutoCADRevisionOptions.R2004

dxfdwgCreator1.ViewEditMode = True

dxfdwgCreator1.FlattenAssembly = True

dxfdwgCreator1.ExportScaleValue = 1.0

dxfdwgCreator1.SettingsFile = "C:\Program Files\Siemens\NX1953\dxfdwg\dxfdwg.def"

dxfdwgCreator1.ExportAs = NXOpen.DxfdwgCreator.ExportAsOption.ThreeD

dxfdwgCreator1.ExportSelectionBlock.SelectionScope = NXOpen.ObjectSelector.Scope.SelectedObjects

dxfdwgCreator1.AutoCADRevision = NXOpen.DxfdwgCreator.AutoCADRevisionOptions.R2018

dxfdwgCreator1.ExportFacesAs = NXOpen.DxfdwgCreator.ExportFacesAsOptions.PolylineMesh

dxfdwgCreator1.FlattenAssembly = False

dxfdwgCreator1.InputFile = "C:\Users\PMiller1\git\nx-dxf\1190181A_G1A-web_named_bodies.prt"

theSession.SetUndoMarkName(markId1, "Export AutoCAD DXF/DWG File Dialog")

' ----------------------------------------------
'   Dialog Begin Export AutoCAD DXF/DWG File
' ----------------------------------------------
Dim objects1(112) As NXOpen.NXObject
Dim sketchFeature1 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(13)"), NXOpen.Features.SketchFeature)

Dim sketch1 As NXOpen.Sketch = CType(sketchFeature1.FindObject("ZINC_ENDS"), NXOpen.Sketch)

Dim line1 As NXOpen.Line = CType(sketch1.FindObject("HANDLE R-39609"), NXOpen.Line)

objects1(0) = line1
Dim line2 As NXOpen.Line = CType(sketch1.FindObject("HANDLE R-39625"), NXOpen.Line)

objects1(1) = line2
Dim sketchFeature2 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(28)"), NXOpen.Features.SketchFeature)

Dim sketch2 As NXOpen.Sketch = CType(sketchFeature2.FindObject("ZINC_PICKPTS"), NXOpen.Sketch)

Dim line3 As NXOpen.Line = CType(sketch2.FindObject("HANDLE R-42349"), NXOpen.Line)

objects1(2) = line3
Dim line4 As NXOpen.Line = CType(sketch2.FindObject("HANDLE R-42595"), NXOpen.Line)

objects1(3) = line4
Dim sketchFeature3 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(19)"), NXOpen.Features.SketchFeature)

Dim sketch3 As NXOpen.Sketch = CType(sketchFeature3.FindObject("REF_LINES_FS"), NXOpen.Sketch)

Dim line5 As NXOpen.Line = CType(sketch3.FindObject("HANDLE R-102237"), NXOpen.Line)

objects1(4) = line5
Dim line6 As NXOpen.Line = CType(sketch3.FindObject("HANDLE R-102238"), NXOpen.Line)

objects1(5) = line6
Dim line7 As NXOpen.Line = CType(sketch3.FindObject("HANDLE R-102239"), NXOpen.Line)

objects1(6) = line7
Dim line8 As NXOpen.Line = CType(sketch3.FindObject("HANDLE R-102240"), NXOpen.Line)

objects1(7) = line8
Dim sketchFeature4 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(21)"), NXOpen.Features.SketchFeature)

Dim sketch4 As NXOpen.Sketch = CType(sketchFeature4.FindObject("ZINC_STIFFENERS"), NXOpen.Sketch)

Dim line9 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278481"), NXOpen.Line)

objects1(8) = line9
Dim line10 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278493"), NXOpen.Line)

objects1(9) = line10
Dim line11 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278499"), NXOpen.Line)

objects1(10) = line11
Dim line12 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278511"), NXOpen.Line)

objects1(11) = line12
Dim sketchFeature5 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(41)"), NXOpen.Features.SketchFeature)

Dim sketch5 As NXOpen.Sketch = CType(sketchFeature5.FindObject("ZINC_SPLICE_1"), NXOpen.Sketch)

Dim line13 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747125"), NXOpen.Line)

objects1(12) = line13
objects1(13) = sketch5
Dim line14 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747133"), NXOpen.Line)

objects1(14) = line14
Dim line15 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747160"), NXOpen.Line)

objects1(15) = line15
Dim line16 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747166"), NXOpen.Line)

objects1(16) = line16
Dim line17 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-1825997"), NXOpen.Line)

objects1(17) = line17
Dim sketchFeature6 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(51)"), NXOpen.Features.SketchFeature)

Dim sketch6 As NXOpen.Sketch = CType(sketchFeature6.FindObject("ZINC_BRGSTIFF1"), NXOpen.Sketch)

Dim line18 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5385439"), NXOpen.Line)

objects1(18) = line18
objects1(19) = sketch6
Dim line19 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5385571"), NXOpen.Line)

objects1(20) = line19
Dim line20 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5385989"), NXOpen.Line)

objects1(21) = line20
Dim sketchFeature7 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(53)"), NXOpen.Features.SketchFeature)

Dim sketch7 As NXOpen.Sketch = CType(sketchFeature7.FindObject("ZINC_BRGSTIFF2"), NXOpen.Sketch)

Dim line21 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5387795"), NXOpen.Line)

objects1(22) = line21
objects1(23) = sketch7
Dim line22 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5387927"), NXOpen.Line)

objects1(24) = line22
Dim line23 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5388345"), NXOpen.Line)

objects1(25) = line23
Dim sketchFeature8 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(55)"), NXOpen.Features.SketchFeature)

Dim sketch8 As NXOpen.Sketch = CType(sketchFeature8.FindObject("ZINC_BRGSTIFF3"), NXOpen.Sketch)

Dim line24 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5390150"), NXOpen.Line)

objects1(26) = line24
objects1(27) = sketch8
Dim line25 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5390282"), NXOpen.Line)

objects1(28) = line25
Dim line26 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5390700"), NXOpen.Line)

objects1(29) = line26
Dim sketchFeature9 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(58)"), NXOpen.Features.SketchFeature)

Dim sketch9 As NXOpen.Sketch = CType(sketchFeature9.FindObject("ZINC_JACKSTIFF1"), NXOpen.Sketch)

Dim line27 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5392640"), NXOpen.Line)

objects1(30) = line27
objects1(31) = sketch9
Dim line28 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5393058"), NXOpen.Line)

objects1(32) = line28
Dim sketchFeature10 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(61)"), NXOpen.Features.SketchFeature)

Dim sketch10 As NXOpen.Sketch = CType(sketchFeature10.FindObject("ZINC_INTSTIFF1"), NXOpen.Sketch)

Dim line29 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5395002"), NXOpen.Line)

objects1(33) = line29
objects1(34) = sketch10
Dim line30 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5395421"), NXOpen.Line)

objects1(35) = line30
Dim line31 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-5554224"), NXOpen.Line)

objects1(36) = line31
Dim intersectionCurve1 As NXOpen.Features.IntersectionCurve = CType(workPart.Features.FindObject("INTERSECTION_CURVES(99)"), NXOpen.Features.IntersectionCurve)

Dim line32 As NXOpen.Line = CType(intersectionCurve1.FindObject("CURVE 1"), NXOpen.Line)

objects1(37) = line32
Dim pointFeature1 As NXOpen.Features.PointFeature = CType(workPart.Features.FindObject("POINT(42)"), NXOpen.Features.PointFeature)

Dim point1 As NXOpen.Point = CType(pointFeature1.FindObject("POINT 1"), NXOpen.Point)

objects1(38) = point1
Dim line33 As NXOpen.Line = CType(sketch1.FindObject("HANDLE R-39479"), NXOpen.Line)

objects1(39) = line33
Dim sketchFeature11 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(20)"), NXOpen.Features.SketchFeature)

Dim sketch11 As NXOpen.Sketch = CType(sketchFeature11.FindObject("REF_STIFFENERS"), NXOpen.Sketch)

Dim line34 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-112770"), NXOpen.Line)

objects1(40) = line34
Dim line35 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-112782"), NXOpen.Line)

objects1(41) = line35
Dim line36 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-243607"), NXOpen.Line)

objects1(42) = line36
Dim line37 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-244213"), NXOpen.Line)

objects1(43) = line37
Dim line38 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-244976"), NXOpen.Line)

objects1(44) = line38
Dim line39 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-245582"), NXOpen.Line)

objects1(45) = line39
Dim line40 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278452"), NXOpen.Line)

objects1(46) = line40
Dim line41 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278475"), NXOpen.Line)

objects1(47) = line41
Dim sketchFeature12 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(26)"), NXOpen.Features.SketchFeature)

Dim sketch12 As NXOpen.Sketch = CType(sketchFeature12.FindObject("ZINC_SETLINES"), NXOpen.Sketch)

Dim line42 As NXOpen.Line = CType(sketch12.FindObject("HANDLE R-413352"), NXOpen.Line)

objects1(48) = line42
Dim line43 As NXOpen.Line = CType(sketch12.FindObject("HANDLE R-454389"), NXOpen.Line)

objects1(49) = line43
Dim line44 As NXOpen.Line = CType(sketch12.FindObject("HANDLE R-454394"), NXOpen.Line)

objects1(50) = line44
Dim line45 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747111"), NXOpen.Line)

objects1(51) = line45
Dim line46 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747117"), NXOpen.Line)

objects1(52) = line46
Dim line47 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747145"), NXOpen.Line)

objects1(53) = line47
Dim line48 As NXOpen.Line = CType(sketch5.FindObject("HANDLE R-1747151"), NXOpen.Line)

objects1(54) = line48
Dim sketchFeature13 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(25)"), NXOpen.Features.SketchFeature)

Dim sketch13 As NXOpen.Sketch = CType(sketchFeature13.FindObject("REF_SETLINES"), NXOpen.Sketch)

Dim line49 As NXOpen.Line = CType(sketch13.FindObject("HANDLE R-2494329"), NXOpen.Line)

objects1(55) = line49
Dim line50 As NXOpen.Line = CType(sketch12.FindObject("HANDLE R-2753858"), NXOpen.Line)

objects1(56) = line50
Dim line51 As NXOpen.Line = CType(sketch12.FindObject("HANDLE R-2754178"), NXOpen.Line)

objects1(57) = line51
Dim line52 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-4723801"), NXOpen.Line)

objects1(58) = line52
Dim line53 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-4724949"), NXOpen.Line)

objects1(59) = line53
Dim sketchFeature14 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(50)"), NXOpen.Features.SketchFeature)

Dim sketch14 As NXOpen.Sketch = CType(sketchFeature14.FindObject("BRGSTIFF1REF"), NXOpen.Sketch)

Dim line54 As NXOpen.Line = CType(sketch14.FindObject("HANDLE R-5384727"), NXOpen.Line)

objects1(60) = line54
objects1(61) = sketch14
Dim line55 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5385778"), NXOpen.Line)

objects1(62) = line55
Dim line56 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5386140"), NXOpen.Line)

objects1(63) = line56
Dim sketchFeature15 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(52)"), NXOpen.Features.SketchFeature)

Dim sketch15 As NXOpen.Sketch = CType(sketchFeature15.FindObject("BRGSTIFF2REF"), NXOpen.Sketch)

Dim line57 As NXOpen.Line = CType(sketch15.FindObject("HANDLE R-5387083"), NXOpen.Line)

objects1(64) = line57
objects1(65) = sketch15
Dim line58 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5388134"), NXOpen.Line)

objects1(66) = line58
Dim line59 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5388496"), NXOpen.Line)

objects1(67) = line59
Dim sketchFeature16 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(54)"), NXOpen.Features.SketchFeature)

Dim sketch16 As NXOpen.Sketch = CType(sketchFeature16.FindObject("BRGSTIFF3REF"), NXOpen.Sketch)

Dim line60 As NXOpen.Line = CType(sketch16.FindObject("HANDLE R-5389438"), NXOpen.Line)

objects1(68) = line60
objects1(69) = sketch16
Dim line61 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5390489"), NXOpen.Line)

objects1(70) = line61
Dim line62 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5390851"), NXOpen.Line)

objects1(71) = line62
Dim sketchFeature17 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(57)"), NXOpen.Features.SketchFeature)

Dim sketch17 As NXOpen.Sketch = CType(sketchFeature17.FindObject("JACKSTIFF1REF"), NXOpen.Sketch)

Dim line63 As NXOpen.Line = CType(sketch17.FindObject("HANDLE R-5391794"), NXOpen.Line)

objects1(72) = line63
objects1(73) = sketch17
Dim line64 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5392508"), NXOpen.Line)

objects1(74) = line64
Dim line65 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5392847"), NXOpen.Line)

objects1(75) = line65
Dim line66 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5393209"), NXOpen.Line)

objects1(76) = line66
Dim sketchFeature18 As NXOpen.Features.SketchFeature = CType(workPart.Features.FindObject("SKETCH(60)"), NXOpen.Features.SketchFeature)

Dim sketch18 As NXOpen.Sketch = CType(sketchFeature18.FindObject("INTSTIFF1REF"), NXOpen.Sketch)

Dim line67 As NXOpen.Line = CType(sketch18.FindObject("HANDLE R-5394152"), NXOpen.Line)

objects1(77) = line67
objects1(78) = sketch18
Dim line68 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5394870"), NXOpen.Line)

objects1(79) = line68
Dim line69 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5395209"), NXOpen.Line)

objects1(80) = line69
Dim line70 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5395572"), NXOpen.Line)

objects1(81) = line70
Dim intersectionCurve2 As NXOpen.Features.IntersectionCurve = CType(workPart.Features.FindObject("INTERSECTION_CURVES(98)"), NXOpen.Features.IntersectionCurve)

Dim line71 As NXOpen.Line = CType(intersectionCurve2.FindObject("CURVE 1"), NXOpen.Line)

objects1(82) = line71
Dim line72 As NXOpen.Line = CType(sketch1.FindObject("HANDLE R-39602"), NXOpen.Line)

objects1(83) = line72
Dim line73 As NXOpen.Line = CType(sketch1.FindObject("HANDLE R-39740"), NXOpen.Line)

objects1(84) = line73
Dim line74 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-112764"), NXOpen.Line)

objects1(85) = line74
Dim line75 As NXOpen.Line = CType(sketch11.FindObject("HANDLE R-112776"), NXOpen.Line)

objects1(86) = line75
Dim line76 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278463"), NXOpen.Line)

objects1(87) = line76
Dim line77 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278469"), NXOpen.Line)

objects1(88) = line77
Dim line78 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278487"), NXOpen.Line)

objects1(89) = line78
Dim line79 As NXOpen.Line = CType(sketch4.FindObject("HANDLE R-278505"), NXOpen.Line)

objects1(90) = line79
Dim line80 As NXOpen.Line = CType(sketch13.FindObject("HANDLE R-393649"), NXOpen.Line)

objects1(91) = line80
Dim line81 As NXOpen.Line = CType(sketch14.FindObject("HANDLE R-5384513"), NXOpen.Line)

objects1(92) = line81
Dim line82 As NXOpen.Line = CType(sketch14.FindObject("HANDLE R-5384792"), NXOpen.Line)

objects1(93) = line82
Dim line83 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5384962"), NXOpen.Line)

objects1(94) = line83
Dim line84 As NXOpen.Line = CType(sketch6.FindObject("HANDLE R-5385210"), NXOpen.Line)

objects1(95) = line84
Dim line85 As NXOpen.Line = CType(sketch15.FindObject("HANDLE R-5386869"), NXOpen.Line)

objects1(96) = line85
Dim line86 As NXOpen.Line = CType(sketch15.FindObject("HANDLE R-5387148"), NXOpen.Line)

objects1(97) = line86
Dim line87 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5387318"), NXOpen.Line)

objects1(98) = line87
Dim line88 As NXOpen.Line = CType(sketch7.FindObject("HANDLE R-5387566"), NXOpen.Line)

objects1(99) = line88
Dim line89 As NXOpen.Line = CType(sketch16.FindObject("HANDLE R-5389224"), NXOpen.Line)

objects1(100) = line89
Dim line90 As NXOpen.Line = CType(sketch16.FindObject("HANDLE R-5389503"), NXOpen.Line)

objects1(101) = line90
Dim line91 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5389673"), NXOpen.Line)

objects1(102) = line91
Dim line92 As NXOpen.Line = CType(sketch8.FindObject("HANDLE R-5389921"), NXOpen.Line)

objects1(103) = line92
Dim line93 As NXOpen.Line = CType(sketch17.FindObject("HANDLE R-5391580"), NXOpen.Line)

objects1(104) = line93
Dim line94 As NXOpen.Line = CType(sketch17.FindObject("HANDLE R-5391859"), NXOpen.Line)

objects1(105) = line94
Dim line95 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5392031"), NXOpen.Line)

objects1(106) = line95
Dim line96 As NXOpen.Line = CType(sketch9.FindObject("HANDLE R-5392279"), NXOpen.Line)

objects1(107) = line96
Dim line97 As NXOpen.Line = CType(sketch18.FindObject("HANDLE R-5393937"), NXOpen.Line)

objects1(108) = line97
Dim line98 As NXOpen.Line = CType(sketch18.FindObject("HANDLE R-5394217"), NXOpen.Line)

objects1(109) = line98
Dim line99 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5394393"), NXOpen.Line)

objects1(110) = line99
Dim line100 As NXOpen.Line = CType(sketch10.FindObject("HANDLE R-5394641"), NXOpen.Line)

objects1(111) = line100
Dim body1 As NXOpen.Body = CType(workPart.Bodies.FindObject("SPLIT BODY(104)1"), NXOpen.Body)

objects1(112) = body1
Dim added1 As Boolean = Nothing
added1 = dxfdwgCreator1.ExportSelectionBlock.SelectionComp.Add(objects1)

' ----------------------------------------------
'   Dialog Begin Export AutoCAD DXF/DWG File
' ----------------------------------------------
Dim markId2 As NXOpen.Session.UndoMarkId = Nothing
markId2 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Export AutoCAD DXF/DWG File")

theSession.DeleteUndoMark(markId2, Nothing)

Dim markId3 As NXOpen.Session.UndoMarkId = Nothing
markId3 = theSession.SetUndoMark(NXOpen.Session.MarkVisibility.Invisible, "Export AutoCAD DXF/DWG File")

dxfdwgCreator1.OutputFile = "T:\SNDataPrd\DXF\1190181A_G1A-w1.dxf"

dxfdwgCreator1.TextFontMappingFile = "C:\Users\PMiller1\AppData\Local\Temp\pmil1C48CBA6q8zi.txt"

dxfdwgCreator1.WidthFactorMode = NXOpen.DxfdwgCreator.WidthfactorMethodOptions.AutomaticCalculation

dxfdwgCreator1.CrossHatchMappingFile = "C:\Users\PMiller1\AppData\Local\Temp\pmil1C48CBA6q8zj.txt"

dxfdwgCreator1.LineFontMappingFile = "C:\Users\PMiller1\AppData\Local\Temp\pmil1C48CBA6q8zk.txt"

dxfdwgCreator1.LayerMask = "1-256"

dxfdwgCreator1.DrawingList = ""

dxfdwgCreator1.ViewList = ""

dxfdwgCreator1.ProcessHoldFlag = True

Dim nXObject1 As NXOpen.NXObject = Nothing
nXObject1 = dxfdwgCreator1.Commit()

theSession.DeleteUndoMark(markId3, Nothing)

theSession.SetUndoMarkName(markId1, "Export AutoCAD DXF/DWG File")

dxfdwgCreator1.Destroy()

' ----------------------------------------------
'   Menu: Tools->Journal->Stop Recording
' ----------------------------------------------

End Sub
End Module