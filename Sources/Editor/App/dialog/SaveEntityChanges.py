from PyQt6.QtWidgets import QMessageBox

class SaveEntityChanges(QMessageBox):
    def __init__(self, entity):
        super().__init__()
        self._entity = entity
        self.setIcon(QMessageBox.Question)
        self.setWindowTitle("Save Entity Changes")
        self.setText("Save changes of entity <b>{0}</b>".format(self._entity.getName()))
        self.setInformativeText("Canges cannot be restored")
        self.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel | QMessageBox.No)