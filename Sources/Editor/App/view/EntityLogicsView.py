from PyQt5.QtWidgets import QWidget, QPushButton, QVBoxLayout, QFrame, QScrollArea

from .LogicView import LogicView
from utils.ViewUtils import ClearLayout
from utils.EventManager import GetEventManager

class EntityLogicsView(QWidget):
    def __init__(self):
        super().__init__()
        self._editEntity = None

        self._rootLayout = QVBoxLayout()

        self._addLogicBt = QPushButton("Add Logic")
        self._addLogicBt.setEnabled(False)
        self._addLogicBt.clicked.connect(self._signal_addLogicBt_clicked)
        self._rootLayout.addWidget(self._addLogicBt)

        self._logicsLayot = QVBoxLayout()

        self._frame = QFrame()
        self._frameLayout = QVBoxLayout()
        self._frameLayout.addLayout(self._logicsLayot)
        self._frameLayout.addStretch()
        self._frame.setLayout(self._frameLayout)

        self._scroll = QScrollArea()
        self._scroll.setWidget(self._frame)
        self._scroll.setWidgetResizable(True)

        self._rootLayout.addWidget(self._scroll)

        self.setLayout(self._rootLayout)

    def _buildLogicsList(self):
        ClearLayout(self._logicsLayot)
        for logic in self._editEntity.getLogics():
            logicView = LogicView(logic)
            self._logicsLayot.addWidget(logicView)

    def setEditEntity(self, entity):
        self._editEntity = entity
        if self._editEntity is not None:
            self._addLogicBt.setEnabled(True)
        else:
            self._addLogicBt.setEnabled(False)
        self._buildLogicsList()

    def addLogicView(self, entityLogic):
        logicView = LogicView(entityLogic)
        self._logicsLayot.addWidget(logicView)

    def _signal_addLogicBt_clicked(self):
        GetEventManager().onAddLogicBtClicked(self._editEntity)