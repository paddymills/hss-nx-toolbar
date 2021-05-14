
import re
from string import ascii_lowercase

from .filter import body_filter

WEB_REGEX = re.compile(r"[-_]web")

def get_body_names(part, bodies):

    # single body
    if len(bodies) == 1:
        return { bodies[0].JournalIdentifier: part.Leaf }

    # web naming
    if WEB_REGEX.search(part.Leaf):
        return _name_web_bodies(part, bodies)

    # generic multibody (-a, -b, -c, etc)
    result = {}
    for i, body in enumerate(bodies):
        result[ body.JournalIdentifier ] = "{}-{}".format(part.Leaf, ascii_lowercase[i])

    return result

def _name_web_bodies(part, bodies):

    result = {}

    for i, body in enumerate(bodies):
        result[ body.JournalIdentifier ] = "{}-W{}".format(part.Leaf, i+1)

    pass
