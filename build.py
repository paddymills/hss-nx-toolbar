
import zipapp
from datetime import datetime

zipapp.create_archive("src", "menus\\application\\hss_dxf_export.zip")

print("[{}] build complete.".format(datetime.now().isoformat()))
