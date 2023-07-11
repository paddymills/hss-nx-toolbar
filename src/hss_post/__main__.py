
from postprocessor import PostProcessor

import NXOpen

try:
    PostProcessor().postprocess()
except Exception as e:
    NXOpen.UI.GetUI().NXMessageBox.Show("Posting Error", NXOpen.NXMessageBox.DialogType.Error, str(e))

