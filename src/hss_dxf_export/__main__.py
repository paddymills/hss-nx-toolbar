
import config
import dialog
from processing import Processor

import argparse
from datetime import date
import logging
import os

import NXOpen

session = NXOpen.Session.GetSession()
processor = Processor()

# setup logging
user = os.getlogin()
timestamp = date.today().isoformat()

# add custom log level "TRACE"
TRACE = 5
logging.addLevelName(TRACE, "TRACE")
def trace(self, message, *args, **kw):
    self.log(TRACE, message, *args, **kw)
logging.Logger.trace = trace


# parse caller options
parser = argparse.ArgumentParser()
parser.add_argument("--select", action="store_true", help="Select parts to export")
parser.add_argument("--work", action="store_true", help="Export work part only")
parser.add_argument("--all_open", action="store_true", help="Export all open parts")
parser.add_argument("--mfg", action="store", nargs="*", help="Export supplied part files")
parser.add_argument("-d", "--dev", action="store_true", help="Development mode")
parser.add_argument("-v", "--verbose", action="count", default=0, help="Increase logging verbosity")

# parse arguments
args, unparsed = parser.parse_known_args()

if session.GetEnvironmentVariableValue("NX_ENV") == "development":
    args.dev = True

if args.dev:
    log_file = r"C:\temp\nx_dxf_export.log"
else:
    log_file = os.path.join( config.LOG_DIR, "{}_{}.log".format( timestamp, user ) )

# Logging level
logging_level = logging.INFO
if args.verbose == 1 or args.dev:
    logging_level = logging.DEBUG
elif args.verbose > 1:
    logging_level = TRACE

logging.basicConfig(
    filename=log_file,
    format='[%(asctime)s]%(levelname)s|%(name)s:%(message)s',
    level=logging_level
)
logger = logging.getLogger(__name__)

logger.info("Process args: {}".format( args ))
if unparsed:
    logger.info("Unparsed args: {}".format( unparsed ))

# log NX version
nx_version = session.GetEnvironmentVariableValue("NX_FULL_VERSION")
if not nx_version:  # NX 12 and prior
    nx_version = session.GetEnvironmentVariableValue("UGII_FULL_VERSION")
logger.info("NX Version: {}".format(nx_version))

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

    # for part in session.Parts:
    for part in session.Parts.GetDisplayedParts():
        processor.add_parts_to_process( part.FullPath )
        parts_added = True

    if not parts_added:     # no files open
        dialog.error("No files are open")

if args.mfg:
    processor.add_parts_to_process( args.mfg )

processor.process_parts()
processor.display_results()
