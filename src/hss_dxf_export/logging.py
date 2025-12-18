
import logging

# add custom log level "TRACE"
TRACE = 5
logging.addLevelName(TRACE, "TRACE")
def trace(self, message, *args, **kw):
    self.log(TRACE, message, *args, **kw)
logging.Logger.trace = trace


logging.basicConfig(
    filename=r"C:\Temp\NXDXF.log",
    format='[%(asctime)s]%(levelname)s|%(name)s:%(message)s',
    level=logging.DEBUG
)
logger = logging.getLogger(__name__)