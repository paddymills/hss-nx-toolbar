
import logging
import re
from os import path
import sys

import config
from nx import DxfExporter, set_top_view

import NXOpen

WEB_REGEX = re.compile(r"[-_]web")


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
        dxf_exporter = DxfExporter(self.session, part.FullPath)

        # get license
        self.session.LicenseManager.Reserve("solid_modeling", "hssdxfexport")

        # make sure modeling is active and set view
        self.session.ApplicationSwitchImmediate("UG_APP_MODELING")
        set_top_view(part)


        try:
            # handle properties

            # handle sketches
            for sketch in self.get_sketches_to_export(part):
                dxf_exporter.add_sketch(sketch)

            # handle bodies
            for name, body in get_bodies_to_export(part):
                dxf_exporter.export_body(body, name, commit=(not self.dry_run))

            self.session.UndoToMark(initial_state, "dxf_initial")

        except Exception as err:
            _, _, tb = sys.exc_info()
            self.logger.error("[{}] {}".format( tb.tb_lineno, err ))

        # release license
        self.session.LicenseManager.Release("solid_modeling", "hssdxfexport")


    def get_sketches_to_export(self, part):
        
        for sk in part.Sketches:
            
            for search_term, layer in config.WHITELISTED_SKETCHES.items():
                if search_term.search(sk.Name):
                    sk.Layer = layer.value
                    yield sk

                    break

            else:
                self.logger.debug("Skipping sketch: {}".format( sk.Name ))


    def get_bodies_to_export(self, part):

        num_bodies = 0
        num_named_bodies = 0
        for body in part.Bodies:
            num_bodies += 1

            if len(body.Name) > 0:
                num_named_bodies += 1

        
        # ~~~~~~~~~~~~~~~~~~~~~~~ single body ~~~~~~~~~~~~~~~~~~~~~~~
        if num_bodies == 1:
            return self._single_body(part)


        # ~~~~~~~~~~~~~~~~~~~~~~~ named bodies ~~~~~~~~~~~~~~~~~~~~~~
        if num_named_bodies > 0:
            return self._named_bodies(part)


        # ~~~~~~~~~~~~~~~~~~~~~~~~ web naming ~~~~~~~~~~~~~~~~~~~~~~~
        if WEB_REGEX.search(part.Leaf):
            return self._name_web_bodies(part)


        # ~~~~~~~~~ generic multibody ( (1), (2), (3), etc. ) ~~~~~~~
        return self._multi_body(part)


    def _single_body(self, part):
        for body in part.Bodies:
            yield part.Leaf, body


    def _named_bodies(self, part):
        for body in part.Bodies:
            if body.Name:
                yield "{}-{}".format(part.Leaf, body.Name), body

            else:
                self.logger.debug("Skipping body that is not named (named body strategy)")


    def _name_web_bodies(self, part):

        for i, body in enumerate(part.Bodies, start=1):
            yield "{}-W{}".format(part.Leaf, i), body


    def _multi_body(self, part):
        for i, body in enumerate(part.Bodies, start=1):
            yield "{}({})".format(part.Leaf, i), body