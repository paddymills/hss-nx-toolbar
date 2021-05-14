
import re
import NXOpen

EMPTY_PROPERTY_PATTERN = re.compile(r"^[ xX]+$")

def is_empty_property(value):
    if EMPTY_PROPERTY_PATTERN.match(value):
        return True

    return False


def set_top_view(part):
    part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)
