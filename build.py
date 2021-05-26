
import os
import shutil
import zipapp

from datetime import datetime

FILEDIALOG_FROM = r"filedialog-rs\target\release\filedialog.dll"
FILEDIALOG_TO = r"menus\application\filedialog.pyd"

# generate filedialog
if not os.path.exists(FILEDIALOG_FROM):
    os.system("cd filedialog-rs && cargo build --release && cd ..")

# copy filedialog
shutil.copy(FILEDIALOG_FROM, FILEDIALOG_TO)

zipapp.create_archive("src", "menus\\application\\hss_dxf_export.zip")

print("[{}] build complete.".format(datetime.now().isoformat()))
