
import logging
import re
import sys

import config
import dialog

from _processor import process_file
from bodies import BodyBound
from dxf_export import DxfExporter
from names import ExportNamer
from properties import get_part_properties
from annotations import add_annotations

import NXOpen
import NXOpen.Layer


class PartProcessor:
    
    def __init__(self):

        self.session = NXOpen.Session.GetSession()
        
        self.parts_to_process = []
        self.result = {
            "success": [],
            "failed": [],
        }

        self.logger = logging.getLogger(__name__)


    @process_file
    def process_part(self, filename):
        # this is the main export script
        # it is wrapped inside _processor.process_open_part()
        # between some misc setup and teardown logic
        #
        # this will export the work part
        #
        # the script:
        #   - handles part properties
        #   - adds sketches to the export
        #   - adds annotations to the export
        #   - exports the bodies
        #   - handles the naming of export files

        part = self.session.Parts.Work

        # initialize dxf/dwg
        dxf_exporter = DxfExporter(part.FullPath)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        #               sketches
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for sketch in part.Sketches:
            if sketch.IsBlanked:
                self.logger.debug("Skipping sketch: {}".format( sketch.Name ))
                continue

            for search_term, layer in config.WHITELISTED_SKETCHES.items():
                if search_term.search(sketch.Name):
                    # set layer
                    for geo in sketch.GetAllGeometry():
                        geo.Layer = layer.value

                    dxf_exporter.add_sketch(sketch)
                    break

            else:
                self.logger.debug("Skipping sketch: {}".format( sketch.Name ))

        

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        #               bodies
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        name_props = get_part_properties(part, props=config.PART_NAME_PROPS)
        name_props["PART_LEAF"] = part.Leaf
        namer = ExportNamer(**name_props)

        props = get_part_properties(part)
        for body in part.Bodies:

            if body.IsBlanked:
                self.logger.debug("Skipping body: {}".format( body.Name or body.JournalIdentifier ))
                continue
            
            if part.IsReadOnly:
                body_pts = None
            else:
                body_pts = BodyBound(body)
                # thickness handling (if bottom face is not on XY plane)
                if body_pts.min_z != 0:
                    self.logger.info("Origin not on XY plane")
                    props["THICKNESS"] = body_pts.max_z - body_pts.min_z
                else:
                    props["THICKNESS"] = None
            
            # export_name = namer.add_export(body, body_pts)
            export_name = part.Leaf

            # skip this body
            if export_name == namer.SKIP_BODY_STR:
                continue


            # create annotation and add to export
            # added_annotations = add_annotations(part, props, body_pts.min_x, body_pts.min_y)
            # added_annotations = add_annotations(part, props, 0.0, 0.0)
            # dxf_exporter.add_annotation( added_annotations )

            # export body
            part.Layers.MoveDisplayableObjects(2, [body])
            dxf_exporter.export_body(body, export_name)

            # stop exporting if SINGLE_BODY_EXPORT_NAME encountered
            if namer.stop_exporting:
                break

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        #               handle export names
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        namer.rename_files()

        # clean-up
        del namer
        del dxf_exporter


    def add_parts_to_process(self, fn):
        if type(fn) is list:
            self.parts_to_process.extend( map(str.lower, fn) )

        else:
            self.parts_to_process.append(fn.lower())


    def process_parts(self):
        
        while self.parts_to_process:
            part = self.parts_to_process.pop(0)

            try:
                self.process_part(part)

                self.result["success"].append(part)

            except Exception as err:
                _, _, tb = sys.exc_info()
                self.logger.error("[{}] {}".format( tb.tb_lineno, err ))
                self.logger.error(err, exc_info=True)

                self.result["failed"].append(part)


    def display_results(self):

        # display nothing if no files processed
        if not any(self.result.values()):
            return

        msg = ["DXF export finished"]

        if self.result["success"]:
            msg.extend(["", "Successful exports:"])
            msg.extend(["  - {}".format(exp) for exp in self.result["success"]])

        if self.result["failed"]:
            msg.extend(["", "Failed exports:"])
            msg.extend(["  - {}".format(exp) for exp in self.result["failed"]])

        dialog.info(msg, "Export Results")
