
import os
import shutil

this_dir = os.path.dirname(__file__)
shutil.copy(os.path.join(this_dir, "target", "release", "filedialog.dll"), os.path.join(this_dir, "filedialog.pyd"))
import filedialog

for f in filedialog.get_files(r"\\hssieng\hssedsserv\Jobs\dxf\test_files"):
    print(f)
