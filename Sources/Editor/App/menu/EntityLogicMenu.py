from PyQt6.QtWidgets import QMenu
from PyQt6.QtGui import QAction

from utils.Managers import GetEventManager
from utils.Managers import GetCopyPasteManager

class EntityLogicMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)

        self._editLogic = None
        self._editEntity = None

        self._copyLogicAct = QAction("Copy Logic")
        self._copyLogicAct.triggered.connect(self._onCopyLogic)

        self._pasteLogicAct = QAction("Paste Logic")
        self._pasteLogicAct.triggered.connect(self._onPasteLogic)

        self.addAction(self._copyLogicAct)
        self.addAction(self._pasteLogicAct)

    def _onCopyLogic(self):
        GetCopyPasteManager().setCopyLogic(self._editLogic)

    def _onPasteLogic(self):
        copyLogic = GetCopyPasteManager().getCopyLogic()
        GetEventManager().onAddCopyLogic(self._editEntity, copyLogic["type"], copyLogic["data"])

    def onMenuRequestedOnLogic(self, pt, logic):
        self._pasteLogicAct.setEnabled(False)
        self._copyLogicAct.setEnabled(True)
        self._editLogic = logic
        self.exec(pt)

    def onMenuRequestedOnEntity(self, pt, entity):
        self._editEntity = entity
        if GetEventManager().hasEditEntity():
            copyLogic = GetCopyPasteManager().getCopyLogic()
            if copyLogic is not None:
                self._pasteLogicAct.setEnabled(True)
            else:
                self._pasteLogicAct.setEnabled(False)
        else:
            self._pasteLogicAct.setEnabled(False)
        self._copyLogicAct.setEnabled(False)
        self.exec(pt)