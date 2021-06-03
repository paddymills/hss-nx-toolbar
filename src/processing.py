
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
                self.logger.error("[{}] {}".format( tb.tb_lineno, err ))
                self.logger.error(err, exc_info=True)

                self.result["failed"].append(part)


    @decorators.process_part
    def process_part(self, filename, close=True):

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
        if body_pts.min_z != 0:
            self.logger.info("Origin not on XY plane")
            attrs["THICKNESS"] = body_pts.max_z - body_pts.min_z

        # note positon
        anno.note_z = 0.0
        if body_pts.len_x < body_pts.len_y:
            # vertical part -> put note to the right of part
            anno.note_x = body_pts.max_x + config.NOTE_OFFSET
            anno.note_y = body_pts.min_y + config.NOTE_OFFSET
            anno.note_size = body_pts.len_y * config.NOTE_SIZE_MULTIPLIER
        else:
            # horizontal/square part -> put note below part
            anno.note_x = body_pts.min_x + config.NOTE_OFFSET
            anno.note_y = body_pts.min_y - config.NOTE_OFFSET
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
            for wl, layer in config.WHITELISTED_SKETCHES:
                if wl.search(sk.Name):
                    exports.sketches.append( sk )
                    layer_moves[layer].append( sk )
                    break

        # add bodies
        i = 1
        for body in work_part.Bodies:

            if body.IsBlanked:
                continue
            
            body_export = SimpleNamespace()
            body_export.body = body

            body_export.anno = self.annotation(body)
            layer_moves[config.Layers.DETAIL].append( body_export.anno )

            if body.Name:

                # skip blacklisted body
                if any([ bl.search(body.Name) for bl in config.BLACKLISTED_BODIES ]):
                    continue

                # single body name -> only export this body
                if body.Name == config.SINGLE_BODY_EXPORT_NAME:
                    body_export.name = self.dxf_export_filename()
                    exports.bodies = [ body_export ]
                    break

                else:
                    body_export.name = self.dxf_export_filename(name=body.Name)

            else:
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


    @decorators.property
    def get_property(self, prop):

        if prop in config.PROPS:
            return config.PROPS[prop]

        elif type(prop) is str:
            return [ prop ]

        # return to wrapper to get property value
        return prop

    
    def dxf_export_filename(self, index=None, name=None):
        EXTENSION = ".dxf"

        if not self.base_name:
            job = self.get_property("JOB")
            mark = self.get_property("MARK")

            if job and mark:
                self.base_name = "{}_{}".format(job, mark)
            else:
                self.base_name = self.work_part.Leaf

        if index:
            dxf_filename = "{}_{}".format(self.base_name, index)

        elif name:
            dxf_filename = "{}-{}".format(self.base_name, name)

        else:
            dxf_filename = self.base_name


        return os.path.join(config.DXF_OUTPUT_DIR, dxf_filename + EXTENSION)


    def move_to_layers(self, layer_map):
        work_part = self.session.Parts.Work

        # ----------------------------------------------
        #   Menu: Format->Move to Layer...
        # ----------------------------------------------
        for layer, entities in layer_map.items():
            self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "Move Layer: {} -> {}".format(layer.name, layer.value))
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
                    change_opt = session.OptionsManager.NewOptionsChangeList(NXOpen.Options.LevelType.User, NXOpen.Options.LevelLockedByDefault.FalseValue)
                    change_opt.SetValue("Assemblies_DisplayReadOnly", 0)
                    change_opt.Save()
                    change_opt.Dispose()

                    dialog.info("Modification warnings turned off. Please restart NX.")

                # will tell part processing to abort
                return True

        # keep this dialog from showing again
        config.HANDLED_READ_ONLY = True
    

if __name__ == '__main__':
    LOGFILE = os.path.join( os.path.dirname(__file__), "test.log" )
    logging.basicConfig(filename=LOGFILE, filemode="w", level=logging.INFO)
    fn = "\\\\hssieng\\jobs\\2019\\1190181f\\eng\\mainmemberparts\\1190181f_x606c.prt"

    Processor().process_part(fn, close=True)
