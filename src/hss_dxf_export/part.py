from abc import ABC, abstractmethod
from tracing import warning, error
from exports import BodyExport
import os
import re

import NXOpen


from tracing import info, debug, http_handler
from config import config


class NxPart(ABC):
    """
    Abstract base class for part processing.

    This class will contain all NX API abstractions and logic.
    """

    def __init__(self, part: NXOpen.Part = None):
        self.session = NXOpen.Session.GetSession()
        self._part = part
        self._properties = None
        self.pre_modify_undo_mark = None

    def __del__(self):
        if self.pre_modify_undo_mark is not None:
            self.session.UndoToMark(self.pre_modify_undo_mark, None)
            self.pre_modify_undo_mark = None
        http_handler.reset_context()

    @property
    def part(self):
        return self._part

    def open_part(self, filename: str) -> NXOpen.Part:
        debug("Opening part: {}".format(filename))
        part, part_load_status = self.session.Parts.OpenActiveDisplay(
            filename, NXOpen.DisplayPartOption.AllowAdditional
        )

        if part_load_status.NumberUnloadedParts > 0:
            _part = part_load_status.GetPartName(0)
            _desc = part_load_status.GetStatusDescription(0)

            raise Exception("{}: {}".format(_desc, _part))

        self._part = part

    def close_part(self):
        debug("Closing part: {}".format(self._part.FullPath))
        if self._part:
            part_close_responses = self.session.Parts.NewPartCloseResponses()
            self._part.Close(
                NXOpen.BasePart.CloseWholeTree.FalseValue,
                NXOpen.BasePart.CloseModified.UseResponses,
                part_close_responses,
            )
            part_close_responses.Dispose()

            self._part = None

        self.session.ApplicationSwitchImmediate("UG_APP_NOPART")

    @property
    def properties(self):
        if self._properties is None:
            self._properties = dict()
            for prop in self.part.GetUserAttributes(False):
                self._properties[prop.Title] = prop.StringValue

        return self._properties

    def export_dxf(self):
        http_handler.set_context("partFile", self.part.FullPath)
        self.pre_modify_undo_mark = self.session.SetUndoMark(
            NXOpen.Session.MarkVisibility.Visible, "Initial State"
        )

        # ensure part is work part
        self.set_work_part()

        # ----------------------------------------------
        #   Menu: File->Export->AutoCAD DXF/DWG...
        # ----------------------------------------------
        self.session.SetUndoMark(
            NXOpen.Session.MarkVisibility.Visible, "AutoCAD DXF/DWG Export"
        )

        dxfdwg_creator = self.session.DexManager.CreateDxfdwgCreator()
        dxfdwg_creator.SettingsFile = os.path.join(
            self.session.GetEnvironmentVariableValue("UGII_CUSTOM_DIR"),
            "DXFDWG",
            "dxfdwg.def",
        )

        dxfdwg_creator.AutoCADRevision = (
            NXOpen.DxfdwgCreator.AutoCADRevisionOptions.R2013
        )
        dxfdwg_creator.WidthFactorMode = (
            NXOpen.DxfdwgCreator.WidthfactorMethodOptions.AutomaticCalculation
        )

        dxfdwg_creator.OutputTo = NXOpen.DxfdwgCreator.OutputToOption.Drafting
        dxfdwg_creator.ExportData = NXOpen.DxfdwgCreator.ExportDataOption.Drawing
        dxfdwg_creator.ExportAs = NXOpen.DxfdwgCreator.ExportAsOption.ThreeD
        dxfdwg_creator.ExportSelectionBlock.SelectionScope = (
            NXOpen.ObjectSelector.Scope.SelectedObjects
        )
        dxfdwg_creator.ExportFacesAs = (
            NXOpen.DxfdwgCreator.ExportFacesAsOptions.PolylineMesh
        )
        dxfdwg_creator.ExportSplinesAs = (
            NXOpen.DxfdwgCreator.ExportSplinesAsOptions.Polyline3D
        )

        dxfdwg_creator.ViewEditMode = True
        dxfdwg_creator.FlattenAssembly = True

        dxfdwg_creator.InputFile = self.part.FullPath

        # prepare output directory
        directory = os.path.join(os.path.dirname(self.part.FullPath), "DXF")
        try:
            if not os.path.exists(directory):
                os.mkdir(directory)
        except PermissionError:
            error("Permission denied: could not create directory {}".format(directory))

        # ----------------------------------------------
        #   Dialog Begin AutoCAD DXF/DWG Export Wizard
        # ----------------------------------------------
        # add body
        for export in self.get_body_exports():

            # add sketches
            for sk in export.sketches:
                dxfdwg_creator.ExportSelectionBlock.SelectionComp.Add(
                    sk.GetAllGeometry()
                )

            # add bodies and annotations
            dxfdwg_creator.ExportSelectionBlock.SelectionComp.Add(export.body)
            dxfdwg_creator.ExportSelectionBlock.SelectionComp.Add(export.anno)

            filename = os.path.join(directory, export.name + ".dxf")
            info("DXF Filename: {}".format(filename))
            dxfdwg_creator.OutputFile = filename
            if os.path.exists(filename):
                os.remove(filename)

            info("Export body: {}".format(export.name))

            dxf_result = dxfdwg_creator.Commit()
            debug("DXF Result: {}".format(dxf_result))

            dxfdwg_creator.ExportSelectionBlock.SelectionComp.Clear()

        dxfdwg_creator.Destroy()

    def add_annotation(self, body: BodyExport):
        # ----------------------------------------------
        #   Menu: Application->Design->Drafting
        # ----------------------------------------------
        self.session.SetUndoMark(
            NXOpen.Session.MarkVisibility.Visible, "Enter Drafting"
        )
        self.session.ApplicationSwitchImmediate("UG_APP_DRAFTING")
        self.part.Drafting.EnterDraftingApplication()

        self.part.Views.WorkView.UpdateCustomSymbols()
        self.part.Drafting.SetTemplateInstantiationIsComplete(True)

        # turn off sheet display
        self.part.Drafting.SetDrawingLayout(False)

        assocOrigin1 = NXOpen.Annotations.Annotation.AssociativeOriginData()

        # create note builder
        note_builder = self.part.Annotations.CreateDraftingNoteBuilder(
            NXOpen.Annotations.SimpleDraftingAid.Null
        )
        note_builder.Origin.SetInferRelativeToGeometry(True)
        note_builder.Origin.Anchor = (
            NXOpen.Annotations.OriginBuilder.AlignmentPosition.TopLeft
        )
        note_builder.Origin.Plane.PlaneMethod = (
            NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane
        )
        note_builder.Style.LetteringStyle.HorizontalTextJustification = (
            NXOpen.Annotations.TextJustification.Left
        )

        note_builder.Text.TextBlock.SetText(body.annotation_text)
        note_builder.Style.LetteringStyle.GeneralTextSize = body.annotation_size
        note_loc = NXOpen.Point3d(*body.annotation_loc)
        note_builder.Origin.Origin.SetValue(
            NXOpen.TaggedObject.Null, NXOpen.View.Null, note_loc
        )

        # create note
        note_result = note_builder.Commit()
        note_builder.Destroy()

        return note_result

    @abstractmethod
    def get_body_exports(self) -> list[BodyExport]:
        # yield a list of bodies to export
        # for each body there will be
        #   - body
        #   - sketches
        #   - dictionary of annotations

        pass

    def __getattr__(self, attr):
        return getattr(self.part, attr)

    def set_work_part(self):
        if self.session.Parts.Work != self.part:
            self.session.Parts.SetActiveDisplay(
                self.part,
                NXOpen.DisplayPartOption.AllowAdditional,
                NXOpen.PartDisplayPartWorkPartOption.UseLast,
            )

    def orient_top_view(self):

        if not self.session.IsBatch:
            # ----------------------------------------------
            #   Menu: Orient View->Top
            # ----------------------------------------------
            self.part.ModelingViews.WorkView.Orient(
                NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit
            )

        # self.session.ApplicationSwitchImmediate("UG_APP_MODELING")

    def move_to_layer(self, layer: int, *objects: NXOpen.DisplayableObject):
        # ----------------------------------------------
        #   Menu: Format->Move to Layer...
        # ----------------------------------------------
        desc = "Move Layer: {} -> {}".format(
            ",".join([obj.Name for obj in objects]), layer
        )
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, desc)

        debug(desc)
        self.part.Layers.MoveDisplayableObjects(layer, objects)


class CadCamPart(NxPart):
    """
    This part is the base class for CAD/CAM parts.

    This will contain the common logic for CAD/CAM parts. This is an
    intermediate class between NxPart and specific CAD/CAM part types.
    """

    def __init__(self, part: NXOpen.Part = None):
        super().__init__(part)
        self._sketches = []

        # prepare part name from file name
        self.cleaned_name = self.part.Leaf.strip()
        for pattern in config.paths.name_strip_patterns:
            self.cleaned_name = re.sub(
                pattern, "", self.cleaned_name, flags=re.IGNORECASE
            )

        # prepare base annotations
        def get_property(vals):
            for key in vals:
                if key in self.properties:
                    return self.properties[key]
            return None

        self.base_anno = {
            "JOB": get_property(config.properties.job),
            "MARK": get_property(config.properties.mark),
            "DRAWING": get_property(config.properties.drawing),
            "MATERIAL": get_property(config.properties.drawing),
        }

        # material grade
        if not self.base_anno["MATERIAL"]:
            spec = self.get_property(config.properties.spec)
            grade = self.get_property(config.properties.grade)
            test = self.get_property(config.properties.test)
            if spec and grade and test:
                self.base_anno["MATERIAL"] = "{}-{}{}".format(spec, grade, test)

    @property
    def export_names(self):
        mapping = dict()

        for k, v in self.properties.items():
            if k.endswith(config.properties.body_name_suffix):
                mapping[k.replace(config.properties.body_name_suffix, "")] = v.strip()

        return mapping

    @property
    def sketches(self):
        if not self._sketches:
            for sk in self.part.Sketches:
                try:
                    if sk.Feature.Suppressed:
                        debug(f"Skipping suppressed sketch: {sk.Name}")
                        continue
                except:
                    warning(
                        f"Skipping sketch: {sk.Name}. (error when querying sketch suppression)"
                    )
                    continue

                debug(f"Possible sketch to export: {sk.Name}")
                layer = config.map_sketch_to_layer(sk.Name)
                if layer:
                    debug(f"Including sketch by `{sk.Name}` on layer {layer}")
                    self._sketches.append(sk)
                    self.move_to_layer(layer, sk)
                    break

        return self._sketches

    def export_dxf(self):
        super().export_dxf()

        info("Exporting CAD/CAM part: {}".format(self.part.FullPath))
        self.set_work_part()

        model_schema_version = int(self.get_property("MODEL_SCHEMA_VERSION") or 0)
        info("Model schema version: {}".format(model_schema_version))

        for key, value in self.get_part_properties():
            info("Property: {} = {}".format(key, value))
        self.orient_top_view()

        # return super().export_dxf()

    def add_annotation(self, body):
        anno = super().add_annotation(body)
        self.move_to_layer(config.layers.detail.layer, anno)

        return anno

    def get_body_exports(self) -> list[BodyExport]:
        exports = []
        next_part_id = 1
        for body in self.part.Bodies:
            if body.Name in config.layers.body.exclude_body_names:
                debug("Skipping excluded body: {}".format(body.Name))
                continue

            if body.IsBlanked:
                debug("Skipping blanked body: {}".format(body.Name))
                continue

            if not body.IsSolidBody and config.layers.body.only_solid_bodies:
                debug("Skipping non solid body: {}".format(body.Name))
                continue

            self.move_to_layer(config.layers.body.layer, body)
            export = BodyExport(body, self.sketches, **self.base_anno)

            # assign export name
            if body.Name is None:
                export.name = self.cleaned_name
            elif body.Name in self.export_names:
                export.name = self.export_names[body.Name]
            else:
                export.name = body.Name

            if body.Name == config.layers.body.single_body_name:
                export.name = self.cleaned_name
                return [export]

            exports.append(export)

        # assign id suffixes if multiple unnamed bodies
        if len(exports) > 1:
            for next_part_id, export in enumerate(exports, start=1):
                if export.name == self.cleaned_name:
                    # assign export name with suffix id
                    export.name = "{}_{}".format(self.cleaned_name, next_part_id)

        return exports


class AlreadyOpenPart(CadCamPart):
    """
    Part that is already open in the session.

    The part only needs to be set as the work part.
    This class is a marker class to differentiate from parts that need to be opened.
    """

    def __init__(self, part: NXOpen.Part):
        if not part:
            raise ValueError("part must be supplied to AlreadyOpenPart")

        super().__init__(part)


class NeedsOpenPart(CadCamPart):
    """
    Part that needs to be opened in the session.

    The part needs to be opened, processed, and then closed.
    """

    def __init__(self, filename: str):
        super().__init__()
        self.filename = filename

    @property
    def part(self):
        if not self._part:
            self.open_part(self.filename)

        return self._part

    def __del__(self):
        super().__del__()
        # ensure part is closed when object is deleted
        self.close_part()
