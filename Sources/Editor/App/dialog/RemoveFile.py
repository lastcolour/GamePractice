from PyQt6.QtWidgets import QMessageBox

class RemoveFile(QMessageBox):
    def __init__(self, fileNode):
        super().__init__()
        self.setIcon(QMessageBox.Question)
        self.setWindowTitle("Remove File")
        self.setText("Remove file: '<b>{0}</b>'".format(fileNode.getRelativePath()))
        self.setInformativeText("Canges cannot be restored")
        self.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel | QMessageBox.No)