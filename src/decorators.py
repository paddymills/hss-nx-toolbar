
from datetime import datetime
import os
import re

import config

import NXOpen
import NXOpen.Annotations
import NXOpen.Drawings


# method decorators (or wrappers) that hold mostly NX logic
# this way the main process script can abstract away most of the NX functions

DT_FORMAT = r"%m-%d-%Y %I:%M:%S %p"


def process_part(func):

    def _impl(self, filename, close=False):
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Load Part")
        
        part_load_status = None
        try:
            # look for part in open parts
            for open_part in self.session.Parts:
                if open_part.FullPath.lower() == filename.lower():
                    # set part as work part
                    self.session.Parts.SetActiveDisplay(open_part, NXOpen.DisplayPartOption.AllowAdditional, NXOpen.PartDisplayPartWorkPartOption.UseLast)
                    close = False

                    break

            else:
                _, part_load_status = self.session.Parts.OpenActiveDisplay(filename, NXOpen.DisplayPartOption.AllowAdditional)
                
                if part_load_status.NumberUnloadedParts > 0:
                    raise("Failed to open part")

        except:
            self.logger.error("Failed to open part: {}".format(filename))
            return

        finally:
            if part_load_status:
                part_load_status.Dispose()


        self.session.ApplicationSwitchImmediate("UG_APP_MODELING")
        
        initial_state = self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Initial State")
        self.logger.info("Processing part: {}".format(self.work_part.FullPath))

        func(self, filename, close)

        if close:
            self.session.UndoToMark(initial_state, None)
            
            part_close_responses = self.session.Parts.NewPartCloseResponses()
            self.work_part.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.UseResponses, part_close_responses)
            
            part_close_responses.Dispose()
            self.session.ApplicationSwitchImmediate("UG_APP_NOPART")


    return _impl


def annotation(func):

    def _impl(self, *args):
        work_part = self.work_part
        
        # ----------------------------------------------
        #   Menu: Application->Design->Drafting
        # ----------------------------------------------
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Enter Drafting")
        self.session.ApplicationSwitchImmediate("UG_APP_DRAFTING")
        work_part.Drafting.EnterDraftingApplication()
        
        # ----------------------------------------------
        #   Menu: Insert->Annotation->Note...
        # ----------------------------------------------
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Note Dialog")
        
        note_builder = work_part.Annotations.CreateDraftingNoteBuilder(NXOpen.Annotations.SimpleDraftingAid.Null)
        note_builder.Origin.Anchor = NXOpen.Annotations.OriginBuilder.AlignmentPosition.MidCenter
        note_builder.Origin.Plane.PlaneMethod = NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane

        # note leader
        leader = work_part.Annotations.CreateLeaderData()
        leader.StubSize = 1.0
        leader.Arrowhead = NXOpen.Annotations.LeaderData.ArrowheadType.FilledArrow
        leader.VerticalAttachment = NXOpen.Annotations.LeaderVerticalAttachment.Center
        note_builder.Leader.Leaders.Append(leader)
        
        # associative origin
        assoc_origin = NXOpen.Annotations.Annotation.AssociativeOriginData()
        assoc_origin.OriginType = NXOpen.Annotations.AssociativeOriginType.Drag
        
        assoc_origin.View = NXOpen.View.Null
        assoc_origin.ViewOfGeometry = NXOpen.View.Null
        assoc_origin.PointOnGeometry = NXOpen.Point.Null
        assoc_origin.AssociatedView = NXOpen.View.Null
        assoc_origin.AssociatedPoint = NXOpen.Point.Null
        assoc_origin.VertAnnotation = NXOpen.Annotations.Annotation.Null
        assoc_origin.HorizAnnotation = NXOpen.Annotations.Annotation.Null
        assoc_origin.AlignedAnnotation = NXOpen.Annotations.Annotation.Null
        assoc_origin.OffsetAnnotation = NXOpen.Annotations.Annotation.Null
        assoc_origin.XOffsetFactor = 0.0
        assoc_origin.YOffsetFactor = 0.0
        assoc_origin.DimensionLine = 0
        
        assoc_origin.VertAlignmentPosition = NXOpen.Annotations.AlignmentPosition.TopLeft
        assoc_origin.HorizAlignmentPosition = NXOpen.Annotations.AlignmentPosition.TopLeft
        assoc_origin.OffsetAlignmentPosition = NXOpen.Annotations.AlignmentPosition.TopLeft
        assoc_origin.StackAlignmentPosition = NXOpen.Annotations.StackAlignmentPosition.Above

        note_builder.Origin.SetAssociativeOrigin(assoc_origin)
        
        data = func(self, *args)
        data.text.append("EXPORTED: {}".format( datetime.now().strftime(DT_FORMAT) ))

        note_builder.Text.TextBlock.SetText(data.text)
        note_builder.Style.LetteringStyle.GeneralTextSize = data.note_size
        note_loc = NXOpen.Point3d(data.note_x, data.note_y, data.note_z)
        note_builder.Origin.Origin.SetValue(NXOpen.TaggedObject.Null, NXOpen.View.Null, note_loc)

        # create note
        note_result = note_builder.Commit()
        note_builder.Destroy()

        return note_result

    def _impl2(self, *args):
        work_part = self.work_part

        # ----------------------------------------------
        #   Menu: Application->Design->Drafting
        # ----------------------------------------------
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Enter Drafting")
        self.session.ApplicationSwitchImmediate("UG_APP_DRAFTING")
        work_part.Drafting.EnterDraftingApplication()

        work_part.Views.WorkView.UpdateCustomSymbols()
        work_part.Drafting.SetTemplateInstantiationIsComplete(True)
        
        # turn off sheet display
        work_part.Drafting.SetDrawingLayout(False)

        assocOrigin1 = NXOpen.Annotations.Annotation.AssociativeOriginData()

        # create note builder
        note_builder = work_part.Annotations.CreateDraftingNoteBuilder(NXOpen.Annotations.SimpleDraftingAid.Null)
        note_builder.Origin.SetInferRelativeToGeometry(True)
        note_builder.Origin.Anchor = NXOpen.Annotations.OriginBuilder.AlignmentPosition.TopLeft
        note_builder.Origin.Plane.PlaneMethod = NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane
        note_builder.Style.LetteringStyle.HorizontalTextJustification = NXOpen.Annotations.TextJustification.Left

        data = func(self, *args)
        data.text.append("EXPORTED: {}".format( datetime.now().strftime(DT_FORMAT) ))

        note_builder.Text.TextBlock.SetText(data.text)
        note_builder.Style.LetteringStyle.GeneralTextSize = data.note_size
        note_loc = NXOpen.Point3d(data.note_x, data.note_y, data.note_z)
        note_builder.Origin.Origin.SetValue(NXOpen.TaggedObject.Null, NXOpen.View.Null, note_loc)

        # create note
        note_result = note_builder.Commit()
        note_builder.Destroy()

        return note_result

    return _impl2


def dwgdxf(func):

    def _impl(self):

        # ----------------------------------------------
        #   Menu: File->Export->AutoCAD DXF/DWG...
        # ----------------------------------------------
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "AutoCAD DXF/DWG Export")
        
        dxfdwg_creator = self.session.DexManager.CreateDxfdwgCreator()
        dxfdwg_creator.SettingsFile = os.path.join( self.session.GetEnvironmentVariableValue("UGII_CUSTOM_DIR"), "DXFDWG", "dxfdwg.def" )
        
        dxfdwg_creator.AutoCADRevision = NXOpen.DxfdwgCreator.AutoCADRevisionOptions.R2013
        dxfdwg_creator.WidthFactorMode = NXOpen.DxfdwgCreator.WidthfactorMethodOptions.AutomaticCalculation

        dxfdwg_creator.OutputTo = NXOpen.DxfdwgCreator.OutputToOption.Drafting
        dxfdwg_creator.ExportData = NXOpen.DxfdwgCreator.ExportDataOption.Drawing
        dxfdwg_creator.ExportAs = NXOpen.DxfdwgCreator.ExportAsOption.ThreeD
        dxfdwg_creator.ExportSelectionBlock.SelectionScope = NXOpen.ObjectSelector.Scope.SelectedObjects
        dxfdwg_creator.ExportFacesAs = NXOpen.DxfdwgCreator.ExportFacesAsOptions.PolylineMesh
        dxfdwg_creator.ExportSplinesAs = NXOpen.DxfdwgCreator.ExportSplinesAsOptions.Polyline3D
        
        dxfdwg_creator.ViewEditMode = True
        dxfdwg_creator.FlattenAssembly = True
        dxfdwg_creator.ExportScaleValue = "1:1"

        dxfdwg_creator.InputFile = self.work_part.FullPath
        
        # ----------------------------------------------
        #   Dialog Begin AutoCAD DXF/DWG Export Wizard 
        # ----------------------------------------------
        
        exports = func(self)

        # add body
        for body_export in exports.bodies:

            # add sketches
            for sk in exports.sketches:
                dxfdwg_creator.ExportSelectionBlock.SelectionComp.Add( sk.GetAllGeometry() )

            # add bodies and annotations
            dxfdwg_creator.ExportSelectionBlock.SelectionComp.Add( body_export.body )
            dxfdwg_creator.ExportSelectionBlock.SelectionComp.Add( body_export.anno )

            dxfdwg_creator.OutputFile = body_export.name

            if os.path.exists( body_export.name ):
                os.remove( body_export.name )
        
            dxf_result  = dxfdwg_creator.Commit()
            dxfdwg_creator.ExportSelectionBlock.SelectionComp.Clear()
        
        dxfdwg_creator.Destroy()
    
    return _impl

def property(func):
    EMPTY_PROPERTY_PATTERN = re.compile(r"^[ xX]+$")

    def _impl(self, *args):

        props = func(self, *args)

        work_part = self.work_part
        for prop in props:
            user_attr_args = (prop, NXOpen.NXObject.AttributeType.Any, -1)

            if work_part.HasUserAttribute(*user_attr_args):
                value = work_part.GetUserAttribute(*user_attr_args).StringValue

                if EMPTY_PROPERTY_PATTERN.match(value):
                    continue

                return value

        return None

    return _impl
