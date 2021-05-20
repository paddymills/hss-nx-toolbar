
import logging

from enum import Enum
from re import compile as regex

class Layers(Enum):
    PROFILE = 1
    MARKING = 2
    NO_CUT  = 3
    DETAIL  = 4


LOGGING_LEVEL = logging.INFO

LOG_DIR = r"\\hssieng\Jobs\dxf\logs"
DXF_OUTPUT_DIR = r"\\hssieng\SNDataPrd\DXF"
NX_PART_FILES_DIR = r"\\hssieng\Jobs"

NOTE_SIZE = 5.0
NOTE_OFFSET = 10.0

PART_NAME_PROPS = {
    "JOB":      ( "JobNo", "JOB_NUMBER" ),
    "MARK":     ( "Mark", "PIECE_MARK" ),
}

ANNOTATION_PROPS = {
    "DRAWING":      ( "DWG_NUMBER" ),
}

SINGLE_BODY_EXPORT_NAME = "SN_PART"

WHITELISTED_SKETCHES =  {
    regex("ZINC")   : Layers.MARKING,
    regex("DOR")    : Layers.NO_CUT,
    regex("NOCUT")  : Layers.NO_CUT,
    regex("NO_CUT") : Layers.NO_CUT,
}

BLACKLISTED_BODIES = (
    regex("SHIM"),
)
