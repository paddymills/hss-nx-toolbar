
import os
import shutil
import zipapp

from argparse import ArgumentParser
from datetime import datetime

# TODO: parse these from ugii_env.dat
DIRS = {
    "local": {
        "menu": r"site\startup",
        "bin": r"site\application",
    },
    "eng": {
        "menu": r"UGALLIANCE\group\Manufacturing\startup",
        "bin": r"UserFunction\application",
    },
}

TARGET_BIN = "target/site/application"
TARGET_MENU = "target/site/startup"

def main():
    ap = ArgumentParser()
    ap.add_argument("-d", "--dest", action="append", help="base nx installation folder to deploy to")
    ap.add_argument("--dev", action="store_true", help="deploy to dev environment")
    ap.add_argument("--no-deploy", action="store_true", help="do not deploy (will not override --dev)")
    ap.add_argument("--rs", action="store_true", help="build rust modules")
    ap.add_argument("-v", "--verbose", action="count", default=0, help="verbosity level")
    args = ap.parse_args()

    if args.verbose > 1:
        print("Args:", args)

    print("📦 packaging...")
    os.makedirs(TARGET_BIN, exist_ok=True)
    if args.rs:
        build_rs()
    build_py()
    shutil.copytree("icons", TARGET_BIN, dirs_exist_ok=True)
    shutil.copytree("menus", TARGET_MENU)

    if args.dev:
        print("🚚 deploying dev...")
        deploy("NX1953", locality="local")
        deploy("NX2007", locality="local")
        # deploy(r"\\hssieng\hssedsserv\Program Files\UGS\NX1953 Test")
    elif not args.no_deploy:
        print("🚚 deploying...")
        for dest_dir in args.dest:
            deploy(dest_dir)

    print("🐍 [{}] build complete.".format(datetime.now().isoformat()))

def build_rs():
    FILEDIALOG_FROM = r"filedialog-rs\target\release\filedialog.dll"
    FILEDIALOG_TO = os.path.join(TARGET_BIN, "filedialog.pyd")

    print("\t🦀 Building filedialog...")

    # generate filedialog
    if not os.path.exists(FILEDIALOG_FROM):
        os.system("cd filedialog-rs && cargo build --release && cd ..")

    # copy filedialog
    shutil.copy(FILEDIALOG_FROM, FILEDIALOG_TO)


def build_py():

    shutil.rmtree("src\\__pycache__", ignore_errors=True)

    """
        For each directory in /src
            1) zip directory into /target
            2) copy {directory}.py into /target
    """
    for entry in os.scandir("src"):
        if entry.is_dir():
            print("\t🗜️  {} zipping...".format(entry.name))

            # copy entrypoint
            shutil.copy("src\\{}.py".format(entry.name), TARGET_BIN)

            # create zip file
            zipapp.create_archive(entry.path, os.path.join(TARGET_BIN, entry.name + ".zip"))


def deploy(base_dir, locality="eng"):
    print("🚚 deploying to {} ...".format(base_dir))

    deploy_dir("nx_ui", os.path.join(base_dir, DIRS[locality]["menu"]))
    print("\t📁 menus")
    
    dest = os.path.join(base_dir, DIRS[locality]["bin"])
    deploy_dir("img", dest)
    print("\t📁 icons")
    deploy_dir("target", dest)
    print("\t📁 code files")


def deploy_dir(src, dest):
    shutil.copytree(src, dest, dirs_exist_ok=True)


if __name__ == "__main__":
    main()
