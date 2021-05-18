
import logging
import re

import config
import NXOpen

WEB_REGEX = re.compile(r"[-_]web")

logger = logging.getLogger(__name__)


def get_bodies_to_export(part):

    num_bodies = 0
    num_named_bodies = 0
    for body in part.Bodies:
        num_bodies += 1

        if body.Name == config.SINGLE_BODY_EXPORT_NAME:
            logger.debug("Naming Strategy: matched single body export name")
            
            return [(part.Leaf, body)]

        if len(body.Name) > 0:
            num_named_bodies += 1

    
    # ~~~~~~~~~~~~~~~~~~~~~~~ single body ~~~~~~~~~~~~~~~~~~~~~~~
    if num_bodies == 1:
        logger.debug("Naming Strategy: single body")

        return _single_body(part)


    # ~~~~~~~~~~~~~~~~~~~~~~~ named bodies ~~~~~~~~~~~~~~~~~~~~~~
    if num_named_bodies > 0:
        logger.debug("Naming Strategy: named bodies")

        return _named_bodies(part)


    # ~~~~~~~~~~~~~~~~~~~~~~~~ web naming ~~~~~~~~~~~~~~~~~~~~~~~
    if WEB_REGEX.search(part.Leaf):
        logger.debug("Naming Strategy: named web bodies")

        return _name_web_bodies(part)


    # ~~~~~~~~~ generic multibody ( (1), (2), (3), etc. ) ~~~~~~~
    logger.debug("Naming Strategy: multiple not-named bodies")
    return _multi_body(part)


def _single_body(part):

    for body in part.Bodies:
        yield part.Leaf, body


def _named_bodies(part):

    for body in part.Bodies:
        if body.Name:
            yield "{}-{}".format(part.Leaf, body.Name), body

        else:
            logger.debug("Skipping body that is not named")


def _name_web_bodies(part):

    for i, body in enumerate(part.Bodies, start=1):
        yield "{}-W{}".format(part.Leaf, i), body


def _multi_body(part):

    for i, body in enumerate(part.Bodies, start=1):
        yield "{}({})".format(part.Leaf, i), body
