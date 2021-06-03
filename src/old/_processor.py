
import logging
import sys

import config
import dialog

import NXOpen
import NXOpen.Options


ABORT_STR = "ABORT_PROCESSING"

# These methods hold the pre and post logic for processing
#   part files (both from a filename and opened part)
#
# This is abstracted away to keep the processing script cleaner

def process_file(wrapped_func):

    def _impl(self, part_file):


        # look for part in open parts
        for open_part in self.session.Parts:
            if open_part.FullPath.lower() == part_file:
                # set part as work part
                self.session.Parts.SetActiveDisplay(open_part, NXOpen.DisplayPartOption.AllowAdditional, NXOpen.PartDisplayPartWorkPartOption.UseLast)
                close_when_done = False

                break

        # open part
        else:
            close_when_done = True

            try:
                _base_part, loadstat = self.session.Parts.OpenActiveDisplay(part_file, NXOpen.DisplayPartOption.AllowAdditional)

                # make sure part opens
                if loadstat.NumberUnloadedParts > 0:
                    self.logger.error("{}: {}".format(loadstat.GetStatusDescription(0), loadstat.GetPartName(0)))
                    dialog.error([
                        "Error opening part: {}".format(loadstat.GetPartName(0)),
                        "",
                        loadstat.GetStatusDescription(0)
                    ])

                    self.result["failed"].append(loadstat.GetPartName(0))
                    
                    raise("Failed to open part")

            except:
                return

            finally:
                loadstat.Dispose()


        part = self.session.Parts.Work

        self.logger.info("Processing part: {}".format(part.Leaf))
        initial_state = self.session.SetUndoMark(NXOpen.Session.MarkVisibility.Visible, "dxf_initial")

        if part.IsReadOnly:
            self.logger.warning("!!! Part is Read Only !!!")

            if handle_read_only(self.session) == ABORT_STR:
                self.parts_to_process.clear()
                return

        # set top view
        if not self.session.IsBatch:
            part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # ~              run processing script              ~
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        wrapped_func(self, part_file)

        # if not part.IsReadOnly:
            # undo to initial state (NX crashes when part is read-only)
        # self.session.UndoToMark(initial_state, "dxf_initial")
        # marksRecycled1, undoUnavailable1 = self.session.UndoLastNVisibleMarks(1)

        # close part
        if close_when_done:
            partCloseResponses1 = self.session.Parts.NewPartCloseResponses()
            part.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.UseResponses, partCloseResponses1)
            part = NXOpen.Part.Null
            partCloseResponses1.Dispose()
            # self.session.Parts.Work.Close(NXOpen.BasePart.CloseWholeTree.FalseValue, NXOpen.BasePart.CloseModified.CloseModified, None)

    return _impl


def handle_read_only(session):

    if config.HANDLED_READ_ONLY:
        return

    # check that Display Message when Modifying Read-Only Parts is not set
    # (Customer Defaults > Assemblies > Miscellaneous > Display Message when Modifying Read-Only Parts)
    read_only_warn_mod = session.OptionsManager.GetIntValue("Assemblies_DisplayReadOnly")
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
            res = dialog.question("Do you want these warnings turned off?", "Turn off warnings")

            # turn off warning for user
            if res == 1:
                change_opt = session.OptionsManager.NewOptionsChangeList(NXOpen.Options.LevelType.User, NXOpen.Options.LevelLockedByDefault.FalseValue)
                change_opt.SetValue("Assemblies_DisplayReadOnly", 0)
                change_opt.Save()
                change_opt.Dispose()

                dialog.info("Modification warnings turned off. Please restart NX.")

            return ABORT_STR

    # keep this dialog from showing again
    config.HANDLED_READ_ONLY = True
