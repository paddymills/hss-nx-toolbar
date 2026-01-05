from abc import ABC, abstractmethod
from argparse import ArgumentParser
from typing import Iterator

import dialog
import NXOpen
from part import AlreadyOpenPart, NeedsOpenPart, NxPart
from tracing import debug, error, info, warning

IGNORE_OPEN_ERRORS = [
    "Failed to find file using current search options, part left unloaded",
]

READ_ONLY_WARNING = """
Display of modification warning for Read-Only parts is turned on.

This means that for every Read-Only part being processed,
\tyou will get a warning that the part was modified during processing.

To suppress this, you need to turn the following option off and restart NX for the change to take affect.
File > Utilities > Customer Defaults > Assemblies > Miscellaneous > Display Message when Modifying Read-Only Parts

Do you want to continue processing parts read-only parts?
"""


class AbstractNxFileProcessor(ABC):
    """
    Abstract base class for different part processing strategies.

    This class will contain all NX API abstractions and logic.
    """

    def __init__(self):
        self.HANDLED_READ_ONLY = False
        self.session = NXOpen.Session.GetSession()

    @property
    @abstractmethod
    def parts_to_process(self) -> Iterator[NxPart]:
        pass

    def run(self):
        success, total = 0, 0
        for part in self.parts_to_process:
            try:
                total += 1

                if part.is_read_only:
                    warning("!!! Part is Read Only !!!")
                    if self.process_read_only == "abort":
                        return

                info("calling exporter")
                part.export_dxf()
                success += 1

            except Exception as e:
                error("Failed to process part: {}".format(e), exc_info=True)

        # this is a new API added in NX 1980
        # TODO: add this to error and warning calls
        NXOpen.UI.GetUI().DisplayNotification(
            "DXF Export",
            "DXF Export complete",
            f"{success}/{total} parts exported.",
            "Information",
        )

    def infer_part_from_filename(
        self, filename: str
    ) -> AlreadyOpenPart | NeedsOpenPart:
        for open_part in self.session.Parts:
            if open_part.FullPath.lower() == filename.lower():
                return AlreadyOpenPart(open_part)
        else:
            return NeedsOpenPart(filename)

    @property
    def process_read_only(self):
        if self.HANDLED_READ_ONLY:
            return True

        # check that Display Message when Modifying Read-Only Parts is not set
        # (Customer Defaults > Assemblies > Miscellaneous > Display Message when Modifying Read-Only Parts)
        read_only_warn_mod = self.session.OptionsManager.GetIntValue(
            "Assemblies_DisplayReadOnly"
        )
        debug("Assemblies Warn Read-Only state: {}".format(read_only_warn_mod))

        if read_only_warn_mod == 1:
            msg = READ_ONLY_WARNING.split("\n")
            abort = not dialog.question(
                msg, "Read-Only modifications will display warning"
            )

            if abort:
                # user chose to abort. make sure nothing else processes
                res = dialog.question(
                    "Do you want these warnings turned off?", "Turn off warnings"
                )

                # turn off warning for user
                if res == "Yes":
                    change_opt = self.session.OptionsManager.NewOptionsChangeList(
                        NXOpen.Options.LevelType.User,
                        NXOpen.Options.LevelLockedByDefault.FalseValue,
                    )
                    change_opt.SetValue("Assemblies_DisplayReadOnly", 0)
                    change_opt.Save()
                    change_opt.Dispose()

                    dialog.info("Modification warnings turned off. Please restart NX.")

                # will tell part processing to abort
                return False

        # keep this dialog from showing again
        self.HANDLED_READ_ONLY = True

        return True


class FilenamePartsProcessor(AbstractNxFileProcessor):
    def __init__(self, parts):
        assert parts, "No parts supplied"
        super().__init__()
        self._parts = parts

    @property
    def parts_to_process(self) -> Iterator[NxPart]:
        for filename in self._parts:
            try:
                part = self.session.Parts.OpenBaseDisplay(filename)
                self.session.Parts.SetActiveDisplay(
                    part,
                    NXOpen.DisplayPartOption.AllowAdditional,
                    NXOpen.PartDisplayPartWorkPartOption.UseLast,
                )
                yield AlreadyOpenPart(part)
            except Exception as e:
                dialog.error("Failed to open part: {}\n{}".format(filename, str(e)))

    def load_or_open_part(self, filename: str) -> NXOpen.Part:
        for open_part in self.session.Parts:
            if open_part.FullPath.lower() == filename.lower():
                # set part as work part
                self.session.Parts.SetActiveDisplay(
                    open_part,
                    NXOpen.DisplayPartOption.AllowAdditional,
                    NXOpen.PartDisplayPartWorkPartOption.UseLast,
                )
                return AlreadyOpenPart(open_part)
        else:
            part, part_load_status = self.session.Parts.OpenActiveDisplay(
                filename, NXOpen.DisplayPartOption.AllowAdditional
            )

            if part_load_status.NumberUnloadedParts > 0:
                _part = part_load_status.GetPartName(0)
                _desc = part_load_status.GetStatusDescription(0)

                if _desc not in IGNORE_OPEN_ERRORS:
                    raise Exception("{}: {}".format(_desc, _part))

            return NeedsOpenPart(part)


class AllOpenPartsProcessor(AbstractNxFileProcessor):
    @property
    def parts_to_process(self) -> Iterator[NxPart]:
        for part in self.session.Parts.GetDisplayedParts():
            yield AlreadyOpenPart(part)


class WorkPartProcessor(AbstractNxFileProcessor):
    @property
    def parts_to_process(self) -> Iterator[NxPart]:
        try:
            yield AlreadyOpenPart(self.session.Parts.Work)
        except Exception as e:
            dialog.error(f"Session does not have a work part ({e})")


def get_processor_from_args() -> AbstractNxFileProcessor:
    # parse caller options
    parser = ArgumentParser()
    parser.add_argument("--select", action="store_true", help="Export selected parts")
    parser.add_argument("--work", action="store_true", help="Export work part only")
    parser.add_argument("--all-open", action="store_true", help="Export all open parts")
    parser.add_argument(
        "--mfg", action="store", nargs="*", help="Export supplied part files"
    )

    # parse arguments
    args, unparsed = parser.parse_known_args()
    debug("Process args: {}".format(args))

    try:
        if unparsed:
            warning("Unparsed args: {}".format(unparsed))

        if args.work:
            return WorkPartProcessor()

        if args.all_open:
            return AllOpenPartsProcessor()

        if args.mfg:
            return FilenamePartsProcessor(args.mfg)

        # default (args.select or no valid args)
        return FilenamePartsProcessor(dialog.get_files_to_process())
    except Exception as e:
        error(e)
        dialog.error("Failed to create processor.\nSee log for details.")
        raise (e)
