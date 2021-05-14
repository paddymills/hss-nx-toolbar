
import logging
import re
from os import path

import config
from nx import NxDxfExporter, set_top_view
from .filter import *

import NXOpen

class HssPartProcessing:
    
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
        self._process_part(self.session.Parts.Work)

        # close part
        self.session.Parts.Work.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.UseResponses, None)
        self.session.ApplicationSwitchImmediate("UG_APP_NOPART")


    def process_parts(self, part_files):
        for part_file in part_files:
            self.process_part(part_file)


    def _process_part(self, part):
        self.logger.info("Processing part: {}".format(part.Leaf))

        initial_state = self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "dxf_initial")

        # initialize dxf/dwg
        dxf_exporter = NxDxfExporter(self.session, part.FullPath)

        # get license
        self.session.LicenseManager.Reserve("solid_modeling", "hssdxfexport")

        # make sure modeling is active and set view
        self.session.ApplicationSwitchImmediate("UG_APP_MODELING")
        set_top_view(part)


        try:
            # handle properties

            # handle sketches
            for sketch in self.get_sketches(part):
                self.logger.info(" + (sketch) {}".format( sketch.Name ))
                dxf_exporter.add_sketch(sketch)

            # handle bodies
            for body in self.get_bodies(part):
                self.logger.info(" > (body) {}".format( body.Name ))

                # TODO: name body
                dxf_exporter.export_body(body, body.Name, commit=(not self.dry_run))

            self.session.UndoToMark(initial_state, "dxf_initial")

        except Exception as err:
            self.logger.error(err)

        # release license
        self.session.LicenseManager.Release("solid_modeling", "hssdxfexport")


    def get_sketches(self, part):
        for sk in part.Sketches:
            
            if filter_sketch(sk.Name):
                yield sk
            else:
                self.logger.debug(" - (sketch) {}".format( sk.Name ))


    def get_bodies(self, part):
        bodies = list()

        for bd in part.Bodies:

            blacklist_id = filter_body(bd.Name)

            if blacklist_id == BodyExport.ONE_BODY:
                return [bd]
            elif blacklist_id == BodyExport.MULT_BODIES:
                bodies.append(bd)

        return bodies
