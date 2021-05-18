
import os
import runpy


this_path = os.path.dirname(__file__)
zip_file = os.path.join(this_path, "hss_dxf_export.zip")

runpy.run_path(zip_file)
