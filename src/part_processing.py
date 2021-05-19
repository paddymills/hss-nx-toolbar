
import logging
import re
from os import path
import sys

import config
from dxf_export import DxfExporter
from sketches import get_sketches_to_export
from bodies import get_bodies_to_export, handle_body_thickness

import NXOpen


class PartProcessor:
    
    def __init__(self):
        self.session = NXOpen.Session.GetSession()
        self.dry_run = False

        self.logger = logging.getLogger(__name__)


    def process_part(self, part_file):
        self.active_part_name = part_file

        # open part
        _, loadstat = self.session.Parts.OpenActiveDisplay(part_file, NXOpen.DisplayPartOption.AllowAdditional)
        loadstat.Dispose()

        # ~~~ run processing script ~~~
        self.process_work_part()

        # close part
        self.session.Parts.Work.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.UseResponses, None)
        self.session.ApplicationSwitchImmediate("UG_APP_NOPART")


    def process_parts(self, part_files):
        for part_file in part_files:
            self.process_part(part_file)


    def process_work_part(self):
        self._process_part(self.session.Parts.Work)


    def process_open_parts(self):
        for part in self.session.Parts:
            # set part as work part
            self.session.Parts.SetActiveDisplay(part, NXOpen.DisplayPartOption.AllowAdditional, NXOpen.PartDisplayPartWorkPartOption.UseLast)
            
            self.process_work_part()


    def _process_part(self, part):
        self.logger.info("Processing part: {}".format(part.Leaf))

        try:
            initial_state = self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "dxf_initial")

            # initialize dxf/dwg
            dxf_exporter = DxfExporter(self.session, part.FullPath)

            # get license
            self.session.LicenseManager.Reserve("solid_modeling", "hssdxfexport")

            # make sure modeling is active and set view
            self.session.ApplicationSwitchImmediate("UG_APP_MODELING")
            part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)


            # handle properties

            # handle sketches
            for sketch in get_sketches_to_export(part):
                dxf_exporter.add_sketch(sketch)

            # handle bodies
            for name, body in get_bodies_to_export(part):
                handle_body_thickness(body)

                dxf_exporter.export_body(body, name, commit=(not self.dry_run))

            self.session.UndoToMark(initial_state, "dxf_initial")

        except Exception as err:
            _, _, tb = sys.exc_info()
            self.logger.error("[{}] {}".format( tb.tb_lineno, err ))

        # release license
        self.session.LicenseManager.Release("solid_modeling", "hssdxfexport")