
from processor import get_processor_from_args

import NXOpen
import tracing

logger = tracing.logger

session = NXOpen.Session.GetSession()

# log NX version
nx_version = session.GetEnvironmentVariableValue("NX_FULL_VERSION")
logger.info("NX Version: {}".format(nx_version))

try:
    get_processor_from_args().run()
except Exception as e:
    logger.error("Fatal error: {}".format(e), exc_info=True)
