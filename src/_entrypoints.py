
import logging
import sys
from os import path, getlogin
from datetime import date

import config
import dialog
from part_processing import PartProcessor
from tests import run_tests

import NXOpen


logger = logging.getLogger(__name__)


def _files_are_open():

    for part in NXOpen.Session.GetSession().Parts:
        return True

    # no files open
    dialog.error("No files are open")
    return False


# Select Part(s)
def select():
    parts_to_process = dialog.get_files_to_process()

    if parts_to_process:
        _process_files(parts_to_process)
        

# Work Part
def active():
    if _files_are_open():
        logger.info("Process work part")
        PartProcessor().process_work_part()

        dialog.info("Work part exported")


# All Open Parts
def all_open():
    if _files_are_open():
        logger.info("Process all open parts")
        PartProcessor().process_open_parts()

        dialog.info("Open parts exported")


# headless -> provide list of parts
def mfg():
    _process_files(sys.argv[2:])


def _process_files(files):
    logger.info("Parts selected to process : {}".format(files))
    result = list( PartProcessor().process_parts(files) )

    if any(result):
        exports = [files[i] for i, x in enumerate(result) if x]

        dialog.info( ["Part file(s) exported:", *exports] )
