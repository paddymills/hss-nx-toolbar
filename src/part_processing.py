
import logging
import re
from os import path
import sys

import config
import dialog

from dxf_export import DxfExporter
from properties import get_part_properties
from sketches import get_sketches_to_export
from bodies import get_bodies_to_export
from body_bound import BodyBound
from annotations import add_annotations

import NXOpen
import NXOpen.Options


class PartProcessor:
    
    def __init__(self):
        self.session = NXOpen.Session.GetSession()

        self.dry_run = False    # for testing

        self.logger = logging.getLogger(__name__)

    
    def __del__(self):
        self.session = None


    def process_part(self, part_file):
        
        # open part
        _, loadstat = self.session.Parts.OpenActiveDisplay(part_file, NXOpen.DisplayPartOption.AllowAdditional)

        # make sure part opens
        if self.session.Parts.Work is None:
            self.logger.error("{}: {}".format(loadstat.GetStatusDescription(0), loadstat.GetPartName(0)))
            dialog.error([
                "Error opening part: {}".format(loadstat.GetPartName(0)),
                "",
                loadstat.GetStatusDescription(0)
            ])
            return False


        # ~~~ run processing script ~~~
        self.process_work_part()

        # close part
        loadstat.Dispose()

        if not self.session.Parts.Work.IsReadOnly:
            self.session.Parts.Work.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.UseResponses, None)
            self.session.ApplicationSwitchImmediate("UG_APP_NOPART")

        return True


    def process_parts(self, part_files):
        
        for part_file in part_files:
            yield self.process_part(part_file)


    def process_work_part(self):
        
        self._process_part(self.session.Parts.Work)


    def process_open_parts(self):
        for part in self.session.Parts:
            # set part as work part
            self.session.Parts.SetActiveDisplay(part, NXOpen.DisplayPartOption.AllowAdditional, NXOpen.PartDisplayPartWorkPartOption.UseLast)
            
            self.process_work_part()


    def _process_part(self, part):

        self.logger.info("Processing part: {}".format(part.Leaf))
        initial_state = self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "dxf_initial")

        if part.IsReadOnly:
            self.logger.warning("!!! Part is Read Only !!!")

            abort = self.handle_read_only()
            if abort:
                return

        try:
            # initial_state = self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "dxf_initial")

            # initialize dxf/dwg
            dxf_exporter = DxfExporter(part.FullPath)

            # make sure modeling is active
            if self.session.ApplicationName != "UG_APP_MODELING":
                self.session.ApplicationSwitchImmediate("UG_APP_MODELING")

            # set top view
            if not (self.session.IsBatch or part.IsReadOnly):
                part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)

            # handle properties
            props = get_part_properties(part)

            # handle sketches
            for sketch in get_sketches_to_export(part):
                dxf_exporter.add_sketch(sketch)

            # handle bodies
            # for name, body in get_bodies_to_export(part):
            for i, body in enumerate(part.Bodies, start=1):
                name = "{}_{}".format(part.Leaf, i)

                # thickness handling (only if NOT read-only)
                _bound = BodyBound(body)

                # if bottom face is not on XY plane
                if _bound.min_z != 0:
                    self.logger.info("Origin not on XY plane")
                    props["THICKNESS"] = _bound.max_z - _bound.min_z

                # create annotation and add to export
                dxf_exporter.add_annotation( add_annotations(part, props, _bound.min_x, _bound.min_y) )

                # export body
                dxf_exporter.export_body(body, name, commit=(not self.dry_run))

            del dxf_exporter
            self.session.UndoToMark(initial_state, "dxf_initial")
            self.session.DeleteAllUndoMarks()

        except Exception as err:
            _, _, tb = sys.exc_info()
            self.logger.error("[{}] {}".format( tb.tb_lineno, err ))

    def handle_read_only(self):
        # check that Display Message when Modifying Read-Only Parts is not set
        # (Customer Defaults > Assemblies > Miscellaneous > Display Message when Modifying Read-Only Parts)
        read_only_warn_mod = self.session.OptionsManager.GetIntValue("Assemblies_DisplayReadOnly")
        self.logger.info("Assemblies Warn Read-Only state: {}".format(read_only_warn_mod))

        if read_only_warn_mod == 1:
            msg = [
                "Display of modification warning for Read-Only parts is turned on.",
                "",
                "This means that for every Read-Only part being processed, ",
                "\tyou will get a warning that the part was modified during processing.",
                "",
                "To suppress this, you need to turn the following option off and restart NX for the change to take affect.",
                "File > Utilities > Customer Defaults > Assemblies > Miscellaneous > Display Message when Modifying Read-Only Parts",
                "",
                "Do you want to continue processing?",
            ]
            res = dialog.question(msg, "Read-Only modifications will display warning")

            if res == 2:
                # user chose to abort. make sure nothing else processes
                res = dialog.question("Do you want these warnings turned off?", "Turn off warnings")

                # turn off warning for user
                if res == 1:
                    change_opt = self.session.OptionsManager.NewOptionsChangeList(NXOpen.Options.LevelType.User, NXOpen.Options.LevelLockedByDefault.FalseValue)
                    change_opt.SetValue("Assemblies_DisplayReadOnly", 0)
                    change_opt.Save()
                    change_opt.Dispose()

                    dialog.info("Modification warnings turned off. Please restart NX.")

                # TODO: Abort current and future processing
                return True

        return False
