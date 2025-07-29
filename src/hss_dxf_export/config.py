
import logging

from enum import Enum
import re
regex = lambda x: re.compile(x, re.IGNORECASE)


class Layers(Enum):
    PROFILE = 1
    MARKING = 2
    NO_CUT  = 3
    DETAIL  = 4


# LOGGING_LEVEL = logging.INFO

LOG_DIR = r"\\hssieng\Jobs\DXF\logs"
DXF_OUTPUT_DIR = r"\\hssieng\SNDataPrd\DXF"
NX_PART_FILES_DIR = r"\\hssieng\Jobs"
DXF_OUTPUT_FALLBACK_DIR = r"\\hssieng\Jobs\DXF\EXPORT"

NOTE_SIZE_MULTIPLIER = 0.0125
NOTE_OFFSET = 10.0

# TODO: settings gui for this option and others (logging level, etc.)
EXPORT_ONLY_SOLID_BODIES = True

SINGLE_BODY_EXPORT_NAME = "SN_PART"

PROP_NAMED_BODIES = ["PARENT", "CHILD1", "CHILD2", "CHILD3", "CHILD4", "CHILD5"]
PROPS = {
    "JOB":      ( "JobNo", "JOB_NUMBER" ),
    "MARK":     ( "Mark", "PIECE_MARK" ),
    "DRAWING":  ( "DWG_NUMBER" ),
    "SPEC":     ( "STEELSPEC" ),
    "GRADE":    ( "STEELGRADE" ),
    "TEST":     ( "STEELTEST" ),
    "PARENT":   ( "PARENT_NAME" ),
    "CHILD1":   ( "CHILD1_NAME" ),
    "CHILD2":   ( "CHILD2_NAME" ),
    "CHILD3":   ( "CHILD3_NAME" ),
    "CHILD4":   ( "CHILD4_NAME" ),
    "CHILD5":   ( "CHILD5_NAME" )
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
