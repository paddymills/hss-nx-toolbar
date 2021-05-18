
import os
import runpy

# This file is necessary because NX will not execute a .zip python file.
# 
# The workaround is for this file to execute the .zip python file.
# This is not entirely necessary, but it keeps the parent folder clean.

exec_dir = os.path.dirname(__file__)
zip_file = os.path.join(exec_dir, "hss_dxf_export.zip")

# will run the __main__.py script within the zip file specified
runpy.run_path(zip_file)
