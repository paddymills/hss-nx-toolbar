
import logging
import re
from os import path

import config
from nx import NxDxfExporter, set_top_view
from .naming import *

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
            for sketch in get_sketches_to_export(part):
                dxf_exporter.add_sketch(sketch)

            # handle bodies
            export_bodies = get_bodies_to_export(part)
            names = get_body_names(export_bodies)
            for body in export_bodies:
                dxf_exporter.export_body(body, names[ body.JournalIdentifier ], commit=(not self.dry_run))

            self.session.UndoToMark(initial_state, "dxf_initial")

        except Exception as err:
            self.logger.error(err)

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
    bodies = list()

    for body in part.Bodies:

        if body.Name.startswith( config.SINGLE_BODY_EXPORT_NAME ):
            return [ body ]

        for blacklist_name in config.BLACKLISTED_BODIES:
            if blacklist_name.search(body.Name):
                bodies.append( body )

    return bodies
