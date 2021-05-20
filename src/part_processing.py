
import logging
import re
from os import path
import sys

import config
from dxf_export import DxfExporter
from properties import get_part_properties
from sketches import get_sketches_to_export
from bodies import get_bodies_to_export
from body_bound import BodyBound
from annotations import add_annotations

import NXOpen


class PartProcessor:
    
    def __init__(self):
        self.session = NXOpen.Session.GetSession()

        self.dry_run = False    # for testing

        self.logger = logging.getLogger(__name__)


    def process_part(self, part_file):
        
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
            dxf_exporter = DxfExporter(part.FullPath)

            # make sure modeling is active and set view
            self.session.ApplicationSwitchImmediate("UG_APP_MODELING")
            part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)

            # handle properties
            props = get_part_properties(part)

            # handle sketches
            for sketch in get_sketches_to_export(part):
                dxf_exporter.add_sketch(sketch)

            # handle bodies
            for name, body in get_bodies_to_export(part):
                _bound = BodyBound(body)

                # if bottom face is not on XY plane
                if _bound.min_z != 0:
                    self.logger.info("Origin not on XY plane")
                    props["THICKNESS"] = _bound.max_z - _bound.min_z

                # create annotation and add to export
                dxf_exporter.add_annotation( add_annotations(part, props, _bound.min_x, _bound.min_y) )

                # export body
                dxf_exporter.export_body(body, name, commit=(not self.dry_run))

            self.session.UndoToMark(initial_state, "dxf_initial")

        except Exception as err:
            _, _, tb = sys.exc_info()
            self.logger.error("[{}] {}".format( tb.tb_lineno, err ))
