
import logging
import re

import config
import NXOpen

logger = logging.getLogger(__name__)


EMPTY_PROPERTY_PATTERN = re.compile(r"^[ xX]+$")


def get_part_properties(part):
    props = {}

    for key in config.JOB_PROP_KEYS:
        prop_val = get_part_property(part, key)

        if not _is_empty_property(prop_val):
            props["JOB"] = prop_val
            break

    for key in config.MARK_PROP_KEYS:
        prop_val = get_part_property(part, key)

        if not _is_empty_property(prop_val):
            props["MARK"] = prop_val
            break

    for key, val in config.PROPS.items():
        prop_val = get_part_property(part, key)

        if not _is_empty_property(prop_val):
            props[val] = prop_val
            break

    return props


def get_part_property(part, prop):

    try:
        return part.GetUserAttribute(prop, NXOpen.NXObject.AttributeType.String, -1).StringValue
    except:
        return None


def _is_empty_property(value):

    if value is None:
        return True

    if EMPTY_PROPERTY_PATTERN.match(value):
        return True

    return False
