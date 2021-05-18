
import NXOpen


def set_top_view(part):
    part.ModelingViews.WorkView.Orient(NXOpen.View.Canned.Top, NXOpen.View.ScaleAdjustment.Fit)
