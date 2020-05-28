from PyQt5.QtWidgets import QWidget, QPushButton, QVBoxLayout, QFrame

from .LogicView import LogicView
from utils.ViewUtils import ClearLayout

class EntityLogicsView(QWidget):
    def __init__(self):
        super().__init__()
        self._editEntity = None

        self._rootLayout = QVBoxLayout()

        self._addLogicBt = QPushButton("Add Logic")
        self._addLogicBt.setEnabled(False)
        self._rootLayout.addWidget(self._addLogicBt)

        self._frame = QFrame()
        self._frameLayout = QVBoxLayout()
        self._frame.setLayout(self._frameLayout)
        self._rootLayout.addWidget(self._frame)

        self.setLayout(self._rootLayout)

    def _buildLogicsList(self):
        ClearLayout(self._frameLayout)
        for logic in self._editEntity.getLogics():
            logicView = LogicView(logic)
            self._frameLayout.addWidget(logicView)

    def setEditEntity(self, entity):
        self._editEntity = entity
        if self._editEntity is not None:
            self._addLogicBt.setEnabled(True)
        else:
            self._addLogicBt.setEnabled(False)
        self._buildLogicsList()