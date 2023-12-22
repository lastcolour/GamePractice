from PyQt6.QtWidgets import QMessageBox

class RemoveFile(QMessageBox):
    def __init__(self, fileNode):
        super().__init__()
        self.setIcon(QMessageBox.Icon.Question)
        self.setWindowTitle("Remove File")
        self.setText("Remove file: '<b>{0}</b>'".format(fileNode.getRelativePath()))
        self.setInformativeText("Canges cannot be restored")
        self.setStandardButtons(QMessageBox.StandardButton.Ok | QMessageBox.StandardButton.Cancel | QMessageBox.StandardButton.No)