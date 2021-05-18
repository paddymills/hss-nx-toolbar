
import logging
import sys
from os import path
from datetime import date

import tkinter as tk
from tkinter import filedialog

import config
import hss
from nx import msgbox

import NXOpen


def main():

    # setup logging
    log_file = path.join( config.LOG_DIR, "{}.log".format(date.today().isoformat()) )
    logging.basicConfig(filename=log_file, level=config.LOGGING_LEVEL, filemode='w')

    processor = hss.PartProcessor()

    # parse caller option
    opt = sys.argv[1]

    # Select Part(s)
    if opt == "select":
        parts_to_process = _get_files_to_process()

        msgbox.info("Process : {}".format(parts_to_process))
        # processor.process_parts(parts_to_process)

    # Work Part
    elif opt == "active":
        if _files_are_open():
            msgbox.info("Process work part")
            # processor.process_work_part()

        else:   # no files open
            msgbox.error("No files are open")


    # All Open Parts
    elif opt == "all_open":
        if _files_are_open():
            msgbox.info("Process all open parts")
            # processor.process_open_parts()

        else:   # no files open
            msgbox.error("No files are open")


    else:
        hss.tests()


def _get_files_to_process():
    root = tk.Tk()
    root.withdraw()

    opts = dict(filetypes=[("NX Parts", "*.prt")], initialdir=config.NX_PART_FILES_DIR)

    return filedialog.askopenfilenames(**opts)


def _files_are_open():

    for part in NXOpen.Session.GetSession().Parts:
        return True

    return False
