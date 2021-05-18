
import NXOpen


def error(msg, title="Error"):
    NXOpen.NXMessageBox.Show(title, NXOpen.NXMessageBox.DialogTypeQuestion.Error, msg)


def warn(msg, title="Warning"):
    NXOpen.NXMessageBox.Show(title, NXOpen.NXMessageBox.DialogTypeQuestion.Warning, msg)


def info(msg, title="Information"):
    NXOpen.NXMessageBox.Show(title, NXOpen.NXMessageBox.DialogTypeQuestion.Information, msg)
