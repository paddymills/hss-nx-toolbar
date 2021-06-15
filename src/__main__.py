
import config
import dialog
from processing import Processor

import argparse
from datetime import date
import logging
import os

import NXOpen

# setup logging
user = os.getlogin()
timestamp = date.today().isoformat()

log_file = os.path.join( config.LOG_DIR, "{}_{}.log".format( timestamp, user ) )

logging.basicConfig(
    filename=log_file,
    format='[%(asctime)s]%(levelname)s|%(name)s:%(message)s',
    level=config.LOGGING_LEVEL
)
logger = logging.getLogger(__name__)


session = NXOpen.Session.GetSession()
processor = Processor()

# log NX version
nx_version = session.GetEnvironmentVariableValue("NX_FULL_VERSION")
if not nx_version:  # NX 12 and prior
    nx_version = session.GetEnvironmentVariableValue("UGII_FULL_VERSION")
logger.info("NX Version: {}".format(nx_version))


# parse caller options
parser = argparse.ArgumentParser()
parser.add_argument("--select", action="store_true")
parser.add_argument("--work", action="store_true")
parser.add_argument("--all_open", action="store_true")
parser.add_argument("--mfg", action="store", nargs="*")

# parse arguments
args, unparsed = parser.parse_known_args()
logger.info("Process args: {}".format( args ))
if unparsed:
    logger.info("Unparsed args: {}".format( unparsed ))

# default -> select
if not any( vars(args).values() ):
    logger.error("No/Invalid process args. Defaulting to select.")
    args.select = True


if args.select:
    processor.add_parts_to_process( dialog.get_files_to_process() )

if args.work:

    try:
        processor.add_parts_to_process( session.Parts.Work.FullPath )
    except:
        dialog.error("Session does not have a work part")

if args.all_open:
    parts_added = False

    for part in session.Parts:
        processor.add_parts_to_process( part.FullPath )
        parts_added = True

    if not parts_added:     # no files open
        dialog.error("No files are open")

if args.mfg:
    processor.add_parts_to_process( args.mfg )

processor.process_parts()
processor.display_results()
