
import config
import re

from enum import Enum

import NXOpen


class BodyExport(Enum):
    SKIP        = 1
    ONE_BODY    = 2
    MULT_BODIES = 3


def filter_sketch(name):
    for search_term, layer in config.WHITELISTED_SKETCHES.items():
        if search_term.search(name):
            return layer

    return 0


def filter_body(name):
    if name.startswith( config.SINGLE_BODY_EXPORT_NAME ):
        return BodyExport.ONE_BODY

    for blacklist_name in config.BLACKLISTED_BODIES:
        if blacklist_name.search(name):
            return BodyExport.SKIP

    return BodyExport.MULT_BODIES
