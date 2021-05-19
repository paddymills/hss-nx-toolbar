
import logging
import re

import config
from hss import properties

import NXOpen

WEB_REGEX = re.compile(r"[-_]web")

logger = logging.getLogger(__name__)


def get_bodies_to_export(part):

    base_export_name = _part_export_name(part)

    num_bodies = 0
    num_named_bodies = 0
    for body in part.Bodies:
        num_bodies += 1

        if body.Name == config.SINGLE_BODY_EXPORT_NAME:
            logger.debug("Naming Strategy: matched single body export name")
            
            return [(base_export_name, body)]

        if len(body.Name) > 0:
            num_named_bodies += 1

    
    # ~~~~~~~~~~~~~~~~~~~~~~~ single body ~~~~~~~~~~~~~~~~~~~~~~~
    if num_bodies == 1:
        logger.debug("Naming Strategy: single body")

        return _single_body(part, base_export_name)


    # ~~~~~~~~~~~~~~~~~~~~~~~ named bodies ~~~~~~~~~~~~~~~~~~~~~~
    if num_named_bodies > 0:
        logger.debug("Naming Strategy: named bodies")

        return _named_bodies(part, base_export_name)


    # ~~~~~~~~~~~~~~~~~~~~~~~~ web naming ~~~~~~~~~~~~~~~~~~~~~~~
    if WEB_REGEX.search(part.Leaf):
        logger.debug("Naming Strategy: named web bodies")

        return _name_web_bodies(part, base_export_name)


    # ~~~~~~~~~ generic multibody ( (1), (2), (3), etc. ) ~~~~~~~
    logger.debug("Naming Strategy: multiple not-named bodies")
    return _multi_body(part, base_export_name)


def handle_part_thickness(part):
    pass


def _part_export_name(part):

    # in case part name has additional information in it
    #       (i.e. '_web', '_PM')
    # we want to build the part name from part properties
    #
    # falls back to part file name if JOB and mark are not found

    props = properties.get_part_properties()

    if "JOB" in props:
        if "MARK" in props:
            return "{}_{}".format(props["JOB"], props["MARK"])
    
    return part.Leaf


def _single_body(part, base_name):

    for body in part.Bodies:
        yield base_name, body


def _named_bodies(part, base_name):

    for body in part.Bodies:
        if body.IsBlanked:
            logger.debug("Skipping blanked body '{}'".format(body.Name))
            continue

        if body.Name:
            yield "{}-{}".format(base_name, body.Name), body

        else:
            logger.debug("Skipping body that is not named")


def _name_web_bodies(part, base_name):

    for i, body in enumerate(part.Bodies, start=1):
        if body.IsBlanked:
            logger.debug("Skipping blanked body '{}'".format(body.Name))
            continue

        yield "{}-W{}".format(base_name, i), body


def _multi_body(part, base_name):

    for i, body in enumerate(part.Bodies, start=1):
        if body.IsBlanked:
            logger.debug("Skipping blanked body '{}'".format(body.Name))
            continue

        yield "{}({})".format(base_name, i), body
