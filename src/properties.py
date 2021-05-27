
import logging
import re

import config
import NXOpen

logger = logging.getLogger(__name__)


EMPTY_PROPERTY_PATTERN = re.compile(r"^[ xX]+$")


def get_part_properties(part, props=config.ANNOTATION_PROPS):
    properties = {}

    for key, _props in props.items():

        properties[key] = get_part_property(part, _props)

    return properties


def get_part_property(part, prop_names):
    
    if type(prop_names) is str:
        prop_names = [prop_names]

    for prop in prop_names:

        result = _get_property_val(part, prop)

        if _is_empty_property(result):
            continue

        return result

    return None


def _get_property_val(part, prop_name):

    try:

        return part.GetUserAttribute(prop_name, NXOpen.NXObject.AttributeType.String, -1).StringValue

    except:

        return None



def _is_empty_property(value):

    if value is None:
        return True

    if EMPTY_PROPERTY_PATTERN.match(value):
        return True

    return False
