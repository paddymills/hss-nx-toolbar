
import NXOpen


def error(msg, title="Error"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Error, title)


def warn(msg, title="Warning"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Warning, title)


def info(msg, title="Information"):

    return _msgbox(msg, NXOpen.NXMessageBox.DialogType.Information, title)


def _msgbox(msg, dlg_type, title):

    return NXOpen.UI.GetUI().NXMessageBox.Show(title, dlg_type, msg)
