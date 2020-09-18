from PyQt5.QtWidgets import QWidget, QPushButton, QVBoxLayout, QFrame, QScrollArea
from PyQt5 import QtCore

from .LogicView import LogicView
from .values.EditTransformValue import EditTransformValue

from utils.ViewUtils import ClearLayout
from utils.EventManager import GetEventManager

from menu.EntityLogicMenu import EntityLogicMenu

class EntityLogicsView(QWidget):
    def __init__(self):
        super().__init__()
        self._editEntity = None

        self._logicMenu = EntityLogicMenu(self)

        self._rootLayout = QVBoxLayout()

        self._transformView = EditTransformValue()
        self._rootLayout.addWidget(self._transformView)

        self._addLogicBt = QPushButton("Add Logic")
        self._addLogicBt.setEnabled(False)
        self._addLogicBt.clicked.connect(self._signal_addLogicBt_clicked)
        self._rootLayout.addWidget(self._addLogicBt)

        self._logicsLayout = QVBoxLayout()

        self._frame = QFrame()
        self._frameLayout = QVBoxLayout()
        self._frameLayout.addLayout(self._logicsLayout)
        self._frameLayout.addStretch()
        self._frame.setLayout(self._frameLayout)
        self._frame.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self._frame.customContextMenuRequested.connect(self._signal_frame_contexMenuRequested)

        self._scroll = QScrollArea()
        self._scroll.setWidget(self._frame)
        self._scroll.setWidgetResizable(True)

        self._rootLayout.addWidget(self._scroll)

        self.setLayout(self._rootLayout)

    def _buildLogicsList(self):
        ClearLayout(self._logicsLayout)
        if self._editEntity is None:
            return
        for logic in self._editEntity.getLogics():
            logicView = LogicView(logic)
            self._logicsLayout.addWidget(logicView)

    def setEditEntity(self, entity):
        self._editEntity = entity
        if self._editEntity is not None:
            self._addLogicBt.setEnabled(True)
        else:
            self._addLogicBt.setEnabled(False)
        self._transformView.setEditEntity(entity)
        self._buildLogicsList()

    def addLogicView(self, entityLogic):
        logicView = LogicView(entityLogic)
        self._logicsLayout.addWidget(logicView)

    def removeLogicView(self, entityLogicId):
        for i in range(self._logicsLayout.count()):
            item = self._logicsLayout.itemAt(i)
            widget = item.widget()
            if widget is not None:
                if widget._entityLogic.getNativeId() == entityLogicId:
                    self._logicsLayout.removeWidget(widget)
                    widget.close()
                    return

    def _signal_addLogicBt_clicked(self):
        GetEventManager().onAddLogicBtClicked(self._editEntity)

    def _signal_frame_contexMenuRequested(self, pt):
        for i in range(self._logicsLayout.count()):
            widget = self._logicsLayout.itemAt(i).widget()
            if widget is not None:
                if widget.underMouse():
                    return
        globalPt = self._frame.mapToGlobal(pt)
        self._logicMenu.onMenuRequestedOnEntity(globalPt, self._editEntity)