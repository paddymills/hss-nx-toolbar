
import logging
import sys
from os import path
from datetime import date

import config
import hss
from nx import dialog

import NXOpen


def _files_are_open():

    for part in NXOpen.Session.GetSession().Parts:
        return True

    return False


# setup logging
log_file = path.join( config.LOG_DIR, "{}.log".format(date.today().isoformat()) )
logging.basicConfig(filename=log_file, level=config.LOGGING_LEVEL, filemode='w')

processor = hss.PartProcessor()


# parse caller option
opt = sys.argv[1]

# Select Part(s)
if opt == "select":
    parts_to_process = dialog.get_files_to_process()

    dialog.info("Process : {}".format(parts_to_process))
    # processor.process_parts(parts_to_process)

# Work Part
elif opt == "active":
    if _files_are_open():
        dialog.info("Process work part")
        # processor.process_work_part()

    else:   # no files open
        dialog.error("No files are open")


# All Open Parts
elif opt == "all_open":
    if _files_are_open():
        dialog.info("Process all open parts")
        # processor.process_open_parts()

    else:   # no files open
        dialog.error("No files are open")


# run tests
elif opt == "test":
    hss.test()
