from PyQt5.QtWidgets import QMenu, QAction

class MainFileMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)

        self._saveChangesAct = QAction("Save Changes")
        self._closeEntityAct = QAction("Close Entity")
        self._exitAct = QAction("Exit")

        self.addAction(self._saveChangesAct)
        self.addAction(self._closeEntityAct)
        self.addAction(self._exitAct)
        self.setTitle("File")