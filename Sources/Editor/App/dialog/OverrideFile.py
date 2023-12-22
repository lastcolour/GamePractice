from PyQt6.QtWidgets import QMessageBox

class OverrideFile(QMessageBox):
    def __init__(self, fileName):
        super().__init__()
        self.setIcon(QMessageBox.Icon.Question)
        self.setWindowTitle("Override File")
        self.setText("Override file: '<b>{0}</b>'".format(fileName))
        self.setInformativeText("Canges cannot be restored")
        self.setStandardButtons(QMessageBox.StandardButton.Ok | QMessageBox.StandardButton.Cancel | QMessageBox.StandardButton.No)