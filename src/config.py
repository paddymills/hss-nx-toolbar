
from enum import Enum
from re import compile as regex

class Layers(Enum):
    PROFILE = 1
    MARKING = 2
    NO_CUT  = 3
    DETAIL  = 4

BASE_DIR = r"C:\Users\pmiller1\git\nx-dxf"
LOG_DIR = BASE_DIR + r"\logs"
DXF_CONFIG = BASE_DIR + r"\config\export.def"
DXF_OUTPUT_DIR = r"\\hssieng\SNDataPrd\DXF"

NOTE_SIZE = 5.0
NOTE_OFFSET = 10.0

JOB_PROP_KEYS = ( "JobNo", "JOB_NUMBER" )
MARK_PROP_KEYS = ( "Mark", "PIECE_MARK" )
PROPS = {
    "DWG_NUMBER": "DRAWING",
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
