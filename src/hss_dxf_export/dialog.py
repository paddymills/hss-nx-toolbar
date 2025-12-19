import os

import config
import tracing

import NXOpen

def error(msg, title="Error"):
    tracing.error(msg)
    return NXOpen.UI.GetUI().NXMessageBox.Show(
        title, NXOpen.NXMessageBox.DialogType.Error, msg
    )

def warn(msg, title="Warning"):
    tracing.warning(msg)
    return NXOpen.UI.GetUI().NXMessageBox.Show(
        title, NXOpen.NXMessageBox.DialogType.Warning, msg
    )

def info(msg, title="Information"):
    tracing.info(msg)
    return NXOpen.UI.GetUI().NXMessageBox.Show(
        title, NXOpen.NXMessageBox.DialogType.Information, msg
    )

def question(msg, title="Question"):
    # question msgbox returns:
    #   -  yes:  1
    #   -   no:  2
    # else, returns -2

    result = NXOpen.UI.GetUI().NXMessageBox.Show(
        title, NXOpen.NXMessageBox.DialogType.Question, msg
    )

    match result:
        case 1:
            return True
        case 2:
            return False
        case _:
            raise Exception("Unexpected dialog result: {}".format(result))


def get_files_to_process():
    import filedialog
    info(config.load_config())

    # get latest load path
    session = NXOpen.Session.GetSession()
    _dir = (
        session.GetEnvironmentVariableValue("_HSS_DXF_LAST_LOAD_DIR")
        or config.NX_PART_FILES_DIR
    )

    # get files from file dialog
    files = filedialog.get_files(_dir)

    # save last file load directory
    try:
        if files:
            session.SetEnvironmentVariableValue(
                "_HSS_DXF_LAST_LOAD_DIR", os.path.dirname(files[0])
            )
    except:
        pass

    return files
