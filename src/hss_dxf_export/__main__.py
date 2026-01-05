import NXOpen
from processor import get_processor_from_args
from tracing import debug, error

session = NXOpen.Session.GetSession()

# log NX version
nx_version = session.GetEnvironmentVariableValue("NX_FULL_VERSION")
debug("NX Version: {}".format(nx_version))

try:
    proc = get_processor_from_args()
    proc.run()
except Exception as e:
    error("Fatal error: {}".format(e), exc_info=True)
