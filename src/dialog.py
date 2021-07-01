
import os

import config
import filedialog

import NXOpen

def ui_dialog(dlg_type):

    def msgbox(func):

        def _impl(*args, **kwargs):
            msg, title = func(*args, **kwargs)

            # if this is a question msgbox, returns:
            #   -  yes:  1
            #   -   no:  2
            # else, returns -2

            return NXOpen.UI.GetUI().NXMessageBox.Show(title, dlg_type, msg)

        return _impl

    return msgbox


@ui_dialog(NXOpen.NXMessageBox.DialogType.Error)
def error(msg, title="Error"):
    return msg, title


@ui_dialog(NXOpen.NXMessageBox.DialogType.Warning)
def warn(msg, title="Warning"):
    return msg, title


@ui_dialog(NXOpen.NXMessageBox.DialogType.Information)
def info(msg, title="Information"):
    return msg, title


@ui_dialog(NXOpen.NXMessageBox.DialogType.Question)
def question(msg, title="Question"):
    return msg, title


def get_files_to_process():

    # get latest load path
    session = NXOpen.Session.GetSession()
    _dir = session.GetEnvironmentVariableValue("_HSS_DXF_LAST_LOAD_DIR") or config.NX_PART_FILES_DIR

    # get files from file dialog
    files = filedialog.get_files( _dir )

    # save last file load directory
    try:
        if files:
            session.SetEnvironmentVariableValue( "_HSS_DXF_LAST_LOAD_DIR", os.path.dirname(files[0]) )
    except:
        pass

    return files
