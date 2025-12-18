from abc import ABC
import logging

import NXOpen

logger = logging.getLogger(__name__)


class NxPart(ABC):
    """
    Abstract base class for part processing.

    This class will contain all NX API abstractions and logic.
    """

    def __init__(self, part: NXOpen.Part = None):
        self.session = NXOpen.Session.GetSession()
        self._part = part
        self._properties = None

    @property
    def part(self):
        return self._part

    @staticmethod
    def open_part(session: NXOpen.Session, filename: str) -> NXOpen.Part:
        part, part_load_status = session.Parts.OpenActiveDisplay(
            filename, NXOpen.DisplayPartOption.AllowAdditional
        )

        if part_load_status.NumberUnloadedParts > 0:
            _part = part_load_status.GetPartName(0)
            _desc = part_load_status.GetStatusDescription(0)

            raise Exception("{}: {}".format(_desc, _part))

        return part

    def close_part(self):
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
            self._properties = self.get_part_properties(self.part)

        return self._properties

    def export_dxf(self):
        model_schema_version = int(self.get_property("MODEL_SCHEMA_VERSION") or 0)
        logger.info("Model schema version: {}".format(model_schema_version))

        self.set_work_part()
        self.orient_top_view()
        # TODO: export dxf logic

    def get_bodies_to_export(self):
        pass

    def get_annotations(self):
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

    def move_to_layer(self, layer: int, *objects: NXOpen.DisplayableObject):
        # ----------------------------------------------
        #   Menu: Format->Move to Layer...
        # ----------------------------------------------
        desc = "Move Layer: {} -> {}".format(
            ",".join([obj.Name for obj in objects]), layer
        )
        self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, desc)

        logger.debug(desc)
        self.part.Layers.MoveDisplayableObjects(layer, objects)


class AlreadyOpenPart(NxPart):
    """
    Part that is already open in the session.

    The part only needs to be set as the work part.
    This class is a marker class to differentiate from parts that need to be opened.
    """


class NeedsOpenPart(NxPart):
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
            self._part = NxPart.open_part(self.session, self.filename)

        return self._part

    def export_dxf(self):
        try:
            # part file will open/load when self.part is accessed
            return super().export_dxf()
        finally:
            # close part (this will happen before returning or raising errors)
            self.close_part()
