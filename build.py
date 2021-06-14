
import os
import sys
import shutil
import zipapp

from datetime import datetime

def build_rs():
    FILEDIALOG_FROM = r"filedialog-rs\target\release\filedialog.dll"
    FILEDIALOG_TO = r"menus\application\filedialog.pyd"

    # generate filedialog
    if not os.path.exists(FILEDIALOG_FROM):
        os.system("cd filedialog-rs && cargo build --release && cd ..")

    # copy filedialog
    shutil.copy(FILEDIALOG_FROM, FILEDIALOG_TO)

    print("Rust build complete and exported")


def build_py():

    shutil.rmtree("src\\__pycache__", ignore_errors=True)

    zipapp.create_archive("src", "menus\\application\\hss_dxf_export.zip")
    zipapp.create_archive("src", "C:\\Users\\PMiller1\\nx\\12\\application\\hss_dxf_export.zip")
    zipapp.create_archive("src", "C:\\Users\\PMiller1\\nx\\1953\\application\\hss_dxf_export.zip")

    print("[{}] build complete.".format(datetime.now().isoformat()))


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "rs":
        build_rs()

    build_py()
