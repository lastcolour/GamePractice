from PyQt5.QtWidgets import QMessageBox

class RemoveEntityLogic(QMessageBox):
    def __init__(self, entityLogic):
        super().__init__()
        self._entityLogic = entityLogic
        self.setIcon(QMessageBox.Question)
        self.setWindowTitle("Remove Entity Logic")
        self.setText("Remove logic <b>'{0}'</b> from entity <b>{1}</b>".format(
            self._entityLogic.getName(), self._entityLogic.getEntity().getName()))
        self.setInformativeText("Canges cannot be restored")
        self.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel | QMessageBox.No)