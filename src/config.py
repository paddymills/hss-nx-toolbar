
import logging

from enum import Enum
import re
regex = lambda x: re.compile(x, re.IGNORECASE)


class Layers(Enum):
    PROFILE = 1
    MARKING = 2
    NO_CUT  = 3
    DETAIL  = 4


LOGGING_LEVEL = logging.INFO

LOG_DIR = r"\\hssieng\Jobs\DXF\logs"
DXF_OUTPUT_DIR = r"\\hssieng\SNDataPrd\DXF"
NX_PART_FILES_DIR = r"\\hssieng\Jobs"

NOTE_SIZE_MULTIPLIER = 0.0125
NOTE_OFFSET = 10.0

SINGLE_BODY_EXPORT_NAME = "SN_PART"

PROPS = {
    "JOB":      ( "JobNo", "JOB_NUMBER" ),
    "MARK":     ( "Mark", "PIECE_MARK" ),
    "DRAWING":  ( "DWG_NUMBER" ),
    "SPEC":     ( "STEELSPEC" ),
    "GRADE":    ( "STEELGRADE" ),
    "TEST":     ( "STEELTEST" ),
}

WHITELISTED_SKETCHES = (
    ( regex("ZINC"),        Layers.MARKING ),
    ( regex("DOR"),         Layers.NO_CUT  ),
    ( regex("NO[-_]?CUT"),  Layers.NO_CUT  ),
)

BLACKLISTED_BODIES = (
    regex("SHIM"),
)


NAME_STRIP_PATTERNS = [
    regex("_mfg"),
    regex("_copy"),
]

HANDLED_READ_ONLY = False
