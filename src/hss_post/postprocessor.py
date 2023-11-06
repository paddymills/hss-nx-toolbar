
import csv

from post import Post
from program import Program

import NXOpen
import NXOpen.Drafting

SAVE_COMPONENTS = NXOpen.BasePart.SaveComponents.TrueValue
CLOSE_AFTER_SAVE = NXOpen.BasePart.CloseAfterSave.FalseValue

class PostProcessor:

    def __init__(self):
        self.session  = NXOpen.Session.GetSession()
        self.logger = self.session.LogFile

        # get posts list
        post_cfg = self.session.GetEnvironmentVariableValue("UGII_CAM_POST_CONFIG_FILE")
        with open(post_cfg) as f:
            posts = [l.split(",")[3] for l in f.read().splitlines() if l.count(",") >= 3]

        with open(post_cfg[:-4] + "_CUSTOM.dat") as custom_as_csv:
            self.posts = [Post(**r, logger=self.log) for r in csv.DictReader(custom_as_csv) if r["PostId"] in posts]

    def log(self, *args):
        self.logger.WriteLine(" ".join(args))

    def log_banner(self, *args):
        self.log("\n\t\t{:*^50}".format(""))
        self.log("\t\t*{:^48}*".format(" ".join(args)))
        self.log("\t\t{:*^50}\n".format(""))

    def postprocess(self):
        part = self.session.Parts.Work

        self.log_banner("POSTPROCESSING STARTED")

        self.session.ApplicationSwitchImmediate("UG_APP_MANUFACTURING")
        self.session.IsCamSessionInitialized()
        
        self.session.CAMSession.PathDisplay.SetAnimationSpeed(5)
        self.session.CAMSession.PathDisplay.SetIpwResolution(NXOpen.CAM.PathDisplay.IpwResolutionType.Medium)
        part.CAMSetup.OutputBallCenter = False
        
        programs = list()
        for program in part.CAMSetup.CAMGroupCollection:
            if type(program) is NXOpen.CAM.NCGroup and len(program.GetMembers()) > 0:
                self.log(">>> Posting program:", program.Name)
                self.session.CAMSession.PathDisplay.ShowToolPath(program)

                # calculate part filename
                prog = Program(part, program, logger=self.log)

                for post in self.posts:
                    if post.is_applicable_post(program):
                        self.log("\t>>> adding post:", post.name)
                        prog.add_post(post)

                programs.append(prog)

        # ensure tool paths are generated
        part.CAMSetup.GenerateToolPath(programs)

        # postprocess all programs
        for program in programs:
            program.post()

        self.export_pdfs(programs)
        self.log_banner("POSTPROCESSING COMPLETE")

        part.Save(SAVE_COMPONENTS, CLOSE_AFTER_SAVE)

    def export_pdfs(self, programs):
        part = self.session.Parts.Work

        # pdf export options
        pdf_factory = part.PlotManager.CreatePrintPdfbuilder()
        pdf_factory.Scale = 1.0
        pdf_factory.Size = NXOpen.PrintPDFBuilder.SizeOption.ScaleFactor
        pdf_factory.OutputText = NXOpen.PrintPDFBuilder.OutputTextOption.Polylines
        pdf_factory.Units = NXOpen.PrintPDFBuilder.UnitsOption.English
        pdf_factory.XDimension = 8.5
        pdf_factory.YDimension = 11.0
        pdf_factory.RasterImages = False
        pdf_factory.ImageResolution = NXOpen.PrintPDFBuilder.ImageResolutionOption.Draft
        pdf_factory.Append = False

        for program in programs:
            for dwg in part.DraftingDrawingSheets:
                if dwg.Name.upper() == program.mfg_program.Name:
                    dwg.Open()
            
                    pdf_factory.SourceBuilder.SetSheets([dwg])
                    for fn in program.pdf_files:
                        pdf_factory.Filename = fn
                        pdf_factory.Commit()

        pdf_factory.Destroy()
