
import os

import config
import filedialog

import NXOpen


def error(msg, title="Error"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Error, title)


def warn(msg, title="Warning"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Warning, title)


def info(msg, title="Information"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Information, title)


def _msgbox(msg, dlg_type, title):

    return NXOpen.UI.GetUI().NXMessageBox.Show(title, dlg_type, msg)


def get_files_to_process():

    # get latest load path
    session = NXOpen.Session.GetSession()
    _dir = session.GetEnvironmentVariableValue("_HSS_DXF_LAST_LOAD_DIR") or config.NX_PART_FILES_DIR

    # get files from file dialog
    files = filedialog.get_files( config.NX_PART_FILES_DIR )

    # save last file load director
    if files:
        session.SetEnvironmentVariableValue( "_HSS_DXF_LAST_LOAD_DIR", os.path.dirname(files[0]) )

    return files
