
import _logging
import dialog
from part_processing import PartProcessor

import argparse
import logging
import sys

import NXOpen

# setup logging
_logging.init()
logger = logging.getLogger(__name__)

nx_version = NXOpen.Session.GetSession().GetEnvironmentVariableValue("NX_FULL_VERSION")
if not nx_version:  # NX 12 and prior
    nx_version = NXOpen.Session.GetSession().GetEnvironmentVariableValue("UGII_FULL_VERSION")
logger.info("NX Version: {}".format(nx_version))

# parse caller options
parser = argparse.ArgumentParser()
parser.add_argument("--select", action="store_true")
parser.add_argument("--work", action="store_true")
parser.add_argument("--all_open", action="store_true")
parser.add_argument("--mfg", action="append")


# parse arguments
args = parser.parse_known_args()[0]
logger.info("Process args: {}".format( args ))

# default -> select
if not any(vars(args).values()):
    logger.error("No/Invalid process args. Defaulting to select.")
    args.select = True


session = NXOpen.Session.GetSession()
processor = PartProcessor()

if args.select:
    logger.info("Select parts to process")
    processor.add_parts_to_process( dialog.get_files_to_process() )

if args.work:
    logger.info("Process work part")

    try:
        processor.add_parts_to_process( session.Parts.Work.FullPath )
    except:
        dialog.error("Session does not have a work part")

if args.all_open:
    logger.info("Process all open parts")
    parts_added = False

    for part in session.Parts:
        processor.add_parts_to_process( part.FullPath )
        parts_added = True

    if not parts_added:     # no files open
        dialog.error("No files are open")

if args.mfg:
    logger.info("Process mfg parts")
    processor.add_parts_to_process( args.mfg )

processor.process_parts()
processor.display_results()
