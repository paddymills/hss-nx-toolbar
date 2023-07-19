
from collections import defaultdict
import logging
from types import SimpleNamespace
import os, sys

from bodies import BodyBound
import config
import dialog
import decorators

import NXOpen
import NXOpen.Features
import NXOpen.Layer


class Processor:

    def __init__(self):
        self.session = NXOpen.Session.GetSession()
        
        self.parts_to_process = []
        self.result = {
            "success": [],
            "failed": [],
        }

        self.logger = logging.getLogger(__name__)


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
                self.logger.error(err, exc_info=True)

                self.result["failed"].append(part)


    @decorators.process_part
    def process_part(self, filename):

        if self.work_part.IsReadOnly:
            self.logger.warning("!!! Part is Read Only !!!")
            if self.handle_read_only():
                return

        self.orient_top_view()
        self.export_dxf()


    @decorators.annotation
    def annotation(self, body):
        anno = SimpleNamespace()

        attrs = {}

        # drawing
        attrs["DRAWING"] = self.get_property("DRAWING")

        # material grade
        spec = self.get_property("SPEC")
        grade = self.get_property("GRADE")
        test = self.get_property("TEST")
        if spec and grade and test:
            attrs["MATERIAL"] = "{}-{}{}".format(spec, grade, test)
        
        # handle body (thickness and note loc/size)
        body_pts = BodyBound(body)
        attrs["THICKNESS"] = body_pts.max_z - body_pts.min_z

        # note position and size
        anno.note_x    = body_pts.min_x + config.NOTE_OFFSET
        anno.note_y    = body_pts.min_y - config.NOTE_OFFSET
        anno.note_z    = 0.0
        anno.note_size = body_pts.len_x * config.NOTE_SIZE_MULTIPLIER

        anno.text = [ "{}: {}".format(k, v) for k, v in attrs.items() if v ]

        #return to wrapper for note creation
        return anno


    @decorators.dwgdxf
    def export_dxf(self):
        work_part = self.work_part
        self.base_name = None

        exports = SimpleNamespace()
        exports.sketches    = []    # exported with every body
        exports.bodies      = []    # exported per each body

        layer_moves = defaultdict(list)

        # add sketches
        for sk in work_part.Sketches:
            if sk.Feature.Suppressed:
                self.logger.debug("Skipping suppressed sketch: {}".format( sk.Name ))
                continue

            for wl, layer in config.WHITELISTED_SKETCHES:
                if wl.search(sk.Name):
                    exports.sketches.append( sk )
                    layer_moves[layer].append( sk )

                    self.logger.info("Adding sketch: {}".format( sk.Name ))
                    break
            else:
                self.logger.debug("Skipping sketch: {}".format( sk.Name ))

        # add bodies
        i = 1
        for body in work_part.Bodies:

            if body.IsBlanked:
                self.logger.debug("Skipping blanked body: {}".format( body.Name ))
                continue

            if not body.IsSolidBody and config.EXPORT_ONLY_SOLID_BODIES:
                self.logger.debug("Skipping non solid body body: {}".format( body.Name ))
                continue
            
            body_export = SimpleNamespace()
            body_export.body = body

            body_export.anno = self.annotation(body)
            layer_moves[config.Layers.DETAIL].append( body_export.anno )

            if body.Name:
                self.logger.debug("Adding body '{}' to exports".format(body.Name))

                # skip blacklisted body
                if any([ bl.search(body.Name) for bl in config.BLACKLISTED_BODIES ]):
                    self.logger.debug("Skipping blacklisted body: {}".format( body.Name ))
                    continue

                # single body name -> only export this body
                if body.Name == config.SINGLE_BODY_EXPORT_NAME:
                    self.logger.debug("Single body name encountered.")
                    body_export.name = self.dxf_export_filename()
                    exports.bodies = [ body_export ]
                    break

                else:
                    body_export.name = self.dxf_export_filename(name=body.Name)

            else:
                self.logger.debug("Adding body '{}' to exports".format(body.JournalIdentifier))
                body_export.name = self.dxf_export_filename(index=i)
                i += 1

            exports.bodies.append(body_export)

        # single body -> follows name of part
        if len(exports.bodies) == 1:
            exports.bodies[0].name = self.dxf_export_filename()

        # add bodies and annotations to layer moves
        layer_moves[config.Layers.PROFILE] = [ b.body for b in exports.bodies ]
        layer_moves[config.Layers.DETAIL]  = [ b.anno for b in exports.bodies ]

        # move entities to layers
        self.move_to_layers(layer_moves)

        # reset base_name
        self.base_name = None

        # return to wrapper for export
        return exports


    @decorators.part_property
    def get_property(self, prop):

        if prop in config.PROPS:
            return config.PROPS[prop]

        elif type(prop) is str:
            return [ prop ]

        # return to wrapper to get property value
        return prop

    
    def dxf_export_filename(self, index=None, name=None):
        
        directory = os.path.join( os.path.dirname( self.work_part.FullPath ), "DXF" )
        try:
            if not os.path.exists(directory):
                os.mkdir(directory)
        except PermissionError:
            directory = config.DXF_OUTPUT_FALLBACK_DIR
            dialog.info([
                "Cannot write to part's parent directory. Will save part to:",
                "\t" + directory,
            ])
        
        if not self.base_name:
            part_name = self.work_part.Leaf

            # job = self.get_property("JOB")
            # mark = self.get_property("MARK")
            # if job and mark and part_name.startswith(job) and mark in part_name:
            #     self.base_name = "{}_{}".format(job, mark)

            self.base_name = part_name
            for pattern in config.NAME_STRIP_PATTERNS:
                self.base_name = pattern.sub( "", self.base_name )

        if index:
            filename = "{}_{}".format(self.base_name, index)

        elif name:
            filename = "{}-{}".format(self.base_name, name)

        else:
            filename = self.base_name

        return os.path.join( directory, filename + ".dxf" )


    def move_to_layers(self, layer_map):
        work_part = self.session.Parts.Work

        # ----------------------------------------------
        #   Menu: Format->Move to Layer...
        # ----------------------------------------------
        for layer, entities in layer_map.items():
            self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Move Layer: {} -> {}".format(layer.name, layer.value))

            self.logger.debug("Move Layer: {} -> {}".format(layer.name, layer.value))
            work_part.Layers.MoveDisplayableObjects( layer.value, entities )

    
    def orient_top_view(self):

        if not self.session.IsBatch:
            # ----------------------------------------------
            #   Menu: Orient View->Top
            # ----------------------------------------------
            self.work_part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)


    @property
    def work_part(self):

        return self.session.Parts.Work


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


    def handle_read_only(self):

        if config.HANDLED_READ_ONLY:
            return

        # check that Display Message when Modifying Read-Only Parts is not set
        # (Customer Defaults > Assemblies > Miscellaneous > Display Message when Modifying Read-Only Parts)
        read_only_warn_mod = self.session.OptionsManager.GetIntValue("Assemblies_DisplayReadOnly")
        logging.getLogger(__name__).debug("Assemblies Warn Read-Only state: {}".format(read_only_warn_mod))

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
                "Do you want to continue processing parts read-only parts?",
            ]
            res = dialog.question(msg, "Read-Only modifications will display warning")

            if res == 2:
                # user chose to abort. make sure nothing else processes
                self.parts_to_process.clear()

                res = dialog.question("Do you want these warnings turned off?", "Turn off warnings")

                # turn off warning for user
                if res == 1:
                    change_opt = self.session.OptionsManager.NewOptionsChangeList(NXOpen.Options.LevelType.User, NXOpen.Options.LevelLockedByDefault.FalseValue)
                    change_opt.SetValue("Assemblies_DisplayReadOnly", 0)
                    change_opt.Save()
                    change_opt.Dispose()

                    dialog.info("Modification warnings turned off. Please restart NX.")

                # will tell part processing to abort
                return True

        # keep this dialog from showing again
        config.HANDLED_READ_ONLY = True
