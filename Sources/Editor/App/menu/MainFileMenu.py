from PyQt6.QtWidgets import QMenu
from PyQt6.QtGui import QAction, QShortcut, QKeySequence

from utils.Managers import GetEventManager

class MainFileMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)

        self._saveChangesAct = QAction("Save Changes")
        self._saveChangesAct.triggered.connect(self._onSaveChanges)
        self._saveChangesAct.setShortcut(QKeySequence("Ctrl+S"))

        self._closeEntityAct = QAction("Close Entity")
        self._closeEntityAct.triggered.connect(self._onCloseEntity)

        self._exitAct = QAction("Exit")
        self._exitAct.triggered.connect(self._onExit)

        self.addAction(self._saveChangesAct)
        self.addAction(self._closeEntityAct)
        self.addAction(self._exitAct)
        self.setTitle("File")

        self.aboutToShow.connect(self._signal_aboutToShow)

    def _signal_aboutToShow(self):
        if GetEventManager().hasEditEntity():
            self._saveChangesAct.setEnabled(True)
            self._closeEntityAct.setEnabled(True)
        else:
            self._saveChangesAct.setEnabled(False)
            self._closeEntityAct.setEnabled(False)

    def _onSaveChanges(self):
        GetEventManager().saveEditEntity()

    def _onCloseEntity(self):
        GetEventManager().closeEditEntity()

    def _onExit(self):
        pass