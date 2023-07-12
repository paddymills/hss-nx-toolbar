
import os
import shutil
import zipapp

from argparse import ArgumentParser
from datetime import datetime

# TODO: fix HSS NX config to align with "local" setup

# TODO: parse these from ugii_env.dat
DIRS = {
    "local": {
        "menu": r"UGALLIANCE\site\startup",
        "bin": r"UGALLIANCE\site\application",
    },
    "eng": {
        "menu": r"UGALLIANCE\group\Manufacturing\startup",
        "bin": r"UserFunction\application",
    },
}

RS_MODULES = [
    "filedialog",
    # "settings-gui",
]

TARGET_DIR = "target/site"
TARGET_BIN = os.path.join(TARGET_DIR, "application")
TARGET_MENU = os.path.join(TARGET_DIR, "startup")

def main():
    ap = ArgumentParser()
    ap.add_argument("-d", "--dest", action="append", help="base NX installation folder to deploy to")
    ap.add_argument("--dev", action="store_true", help="deploy to dev environment")
    ap.add_argument("-l", "--local", action="store_true", help="deploy to local NX installations")
    ap.add_argument("--no-deploy", action="store_true", help="do not deploy (will not override --dev)")
    ap.add_argument("--skip-rs-build", action="store_true", help="skip building rust modules")
    ap.add_argument("-v", "--verbose", action="count", default=0, help="verbosity level")
    args = ap.parse_args()

    if args.verbose > 1:
        print("Args:", args)

    clean()

    print("📦 packaging...")
    shutil.copytree("icons", TARGET_BIN)
    shutil.copytree("menus", TARGET_MENU)
    if not args.skip_rs_build:
        build_rs()
    build_py()

    if args.local:
        print("🚚 deploying local...")
        for entry in os.scandir(r"C:\Program Files\Siemens"):
            if entry.is_dir():
                deploy(entry.path, locality="local")
    if not args.no_deploy:
        print("🚚 deploying...")
        for dest_dir in args.dest:
            deploy(dest_dir)

    print("🐍 [{:%d-%b-%y %I:%M%p}] build complete.".format(datetime.now()))

def clean():
    # reset target folder structure
    if os.path.exists("target"):
        shutil.rmtree("target")
    os.makedirs(TARGET_DIR)

    # remove all __pycache__
    for root, _dirs, _files in os.walk("src"):
        if root.endswith("__pycache__"):
            shutil.rmtree(root)


def build_rs():
    """
        For each directory in /rs-modules
            1) build module
            2) copy {module}.dll to TARGET_BIN/{module}.pyd
    """
    for module in RS_MODULES:
        module_src = "rs-modules/{0}/target/release/{0}.dll".format(module)
        module_dest = os.path.join(TARGET_BIN, "{}.pyd".format(module))

        print("\t🦀 Building {}...".format(module))

        # compile module
        os.system("cargo build --manifest-path=rs-modules/{}/Cargo.toml --quiet --release".format(module))

        # copy filedialog
        shutil.copy(module_src, module_dest)


def build_py():
    """
        For each directory in /src
            1) copy {directory}.py into TARGET_BIN
            2) zip directory into TARGET_BIN
    """
    for entry in os.scandir("src"):
        if entry.is_dir():
            print("\t🗜️  {} zipping...".format(entry.name))

            # copy entrypoint
            shutil.copy("src/{}.py".format(entry.name), TARGET_BIN)

            # create zip file
            zipapp.create_archive(entry.path, os.path.join(TARGET_BIN, entry.name + ".zip"))


def deploy(base_dir, locality="eng"):
    print("🚚 deploying to {} {} ...".format(locality, base_dir))

    deploy_dir("target/site/startup", os.path.join(base_dir, DIRS[locality]["menu"]))
    print("\t📁 menus")
    
    dest = os.path.join(base_dir, DIRS[locality]["bin"])
    deploy_dir("target/site/application", dest)
    print("\t📁 icons")
    print("\t📁 code files")


def deploy_dir(src, dest):
    shutil.copytree(src, dest, dirs_exist_ok=True)


if __name__ == "__main__":
    main()
