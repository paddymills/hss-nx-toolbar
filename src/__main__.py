
import sys
import _entrypoints
import _logging
import logging

import NXOpen


entrypoint_map = {
    "select":       _entrypoints.select,
    "active":       _entrypoints.active,
    "all_open":     _entrypoints.all_open,
    "mfg":          _entrypoints.mfg,
    "test":         _entrypoints.test,
}

# setup logging
_logging.init()
logger = logging.getLogger(__name__)

nx_version = NXOpen.Session.GetSession().GetEnvironmentVariableValue("NX_FULL_VERSION")
if not nx_version:  # NX 12 and prior
    nx_version = NXOpen.Session.GetSession().GetEnvironmentVariableValue("UGII_FULL_VERSION")
logger.info("NX Version: {}".format(nx_version))

# parse caller option
try:
    opt = sys.argv[1]
    logger.info("Process args: {}".format(sys.argv[1:]))

# default -> select
except IndexError:
    opt = "select"
    logger.info("No process args given. Using default ('select').")

entrypoint_map[opt]()