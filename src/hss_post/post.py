
import os

import NXOpen
import NXOpen.CAM

MACHINING_CENTER_GROUP = "MILL_PROGRAM"
SHAPE_GROUP = "PEDDIMAT_PROGRAM"

POST_ARGS = (
    NXOpen.CAM.CAMSetup.PostprocessSettingsOutputWarning.PostDefined,
    NXOpen.CAM.CAMSetup.PostprocessSettingsReviewTool.PostDefined,
    NXOpen.CAM.CAMSetup.PostprocessSettingsPostMode.Normal
)

UNITS = {
    "0": NXOpen.CAM.CAMSetup.OutputUnits.Inch,
    "1": NXOpen.CAM.CAMSetup.OutputUnits.Metric
}

class Post:

    def __init__(self, logger=None, **kwargs):
        self.id = kwargs["PostId"]
        self.name = kwargs["PostName"]
        self.ext = kwargs["Extension"]
        self.group = kwargs["ProgramGroup"]
        self.folder = kwargs["PostFolder"]
        self.mach_id = kwargs["MachId"]

        self.log = logger

    def units(self, part):
        if self.group != MACHINING_CENTER_GROUP:
            try:
                return UNITS[str(part.PartUnits)]
            except KeyError:
                raise ValueError("Unmatched part units: {}".format(part.PartUnits))

        return NXOpen.CAM.CAMSetup.OutputUnits.PostDefined

    @property
    def is_shape_post(self):
        return self.group == SHAPE_GROUP

    def is_applicable_post(self, program):
        is_peddimat_program = SHAPE_GROUP in program.Name

        # if post is peddimat and program name is not PEDDIMAT_PROGRAM
        if self.is_shape_post:
            if not is_peddimat_program:
                return False
        else:
            if is_peddimat_program:
                return False

        return True

    def postprocess(self, part, program, filename):
        # set posting directory
        post_dir = os.path.join(os.path.dirname(part.FullPath), self.folder)

        if not os.path.exists(post_dir):
            os.mkdir(post_dir)

        nc_filename = os.path.join(post_dir, filename + self.ext)

        try:
            part.CAMSetup.PostprocessWithPostModeSetting(
                [program],
                self.name,
                nc_filename,
                self.units(part),
                *POST_ARGS
            )
        except:
            self.log("Posting failed for: {} to {}".format(program.Name, self.name))

        return nc_filename
