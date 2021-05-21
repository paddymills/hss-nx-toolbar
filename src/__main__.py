
import logging
import sys
from os import path, getlogin
from datetime import date

import _logging
import config
import dialog
from part_processing import PartProcessor
from tests import test

import NXOpen


def _files_are_open():

    for part in NXOpen.Session.GetSession().Parts:
        return True

    return False


# setup logging
_logging.init()
logger = logging.getLogger(__name__)

processor = PartProcessor()


# parse caller option
opt = sys.argv[1]

# Select Part(s)
if opt == "select":
    parts_to_process = dialog.get_files_to_process()

    if parts_to_process:

        logger.info("Parts selected to process : {}".format(parts_to_process))
        result = processor.process_parts(parts_to_process)

        if any(result):
            exports = [parts_to_process[i] for i, x in enumerate(result) if x]

            dialog.info( ["Part file(s) exported:", *exports] )

# Work Part
elif opt == "active":
    if _files_are_open():
        logger.info("Process work part")
        processor.process_work_part()

        dialog.info("Work part exported")

    else:   # no files open
        dialog.error("No files are open")


# All Open Parts
elif opt == "all_open":
    if _files_are_open():
        logger.info("Process all open parts")
        processor.process_open_parts()

        dialog.info("Open parts exported")

    else:   # no files open
        dialog.error("No files are open")


# run tests
elif opt == "test":
    test()
