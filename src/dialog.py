
import os
import tkinter as tk
from tkinter import filedialog

import config

import NXOpen

# need Tcl (tcl/{tcl8.6, tk8.6})
os.environ["TCL_LIBRARY"] = r"C:\Program Files\Python39\tcl\tcl8.6"


def error(msg, title="Error"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Error, title)


def warn(msg, title="Warning"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Warning, title)


def info(msg, title="Information"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Information, title)


def _msgbox(msg, dlg_type, title):

    return NXOpen.UI.GetUI().NXMessageBox.Show(title, dlg_type, msg)


def get_files_to_process():
    root = tk.Tk()
    root.withdraw()

    opts = dict(filetypes=[("NX Parts", "*.prt")], initialdir=config.NX_PART_FILES_DIR)

    return filedialog.askopenfilenames(**opts)
