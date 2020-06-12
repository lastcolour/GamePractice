from PyQt5.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QDockWidget
from PyQt5 import QtCore

from utils.Log import Log
from utils.AppConfig import AppConfig
from utils.EventManager import CreateEventManager
from utils.EngineViewManager import CreateEngineViewManager

from view.EntityFileView import EntityFileView
from view.EntityLogicsView import EntityLogicsView
from view.EntityTreeView import EntityTreeView
from view.EngineOutputView import EngineOutputView

from native.EditorNative import EditorNative

from model.AssetsModel import AssetsModel
from model.LogicsModel import LogicsModel

from menu.MainFileMenu import MainFileMenu
from menu.MainToolBar import MainToolBar

from view.main.StatusBar import StatusBar

import sys

class EditorView(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Editor")
        self.setMinimumSize(640, 480)
        self.setContentsMargins(1, 1, 1, 1)

        self._fileMenu = MainFileMenu(self)
        self.menuBar().addMenu(self._fileMenu)
        self.addToolBar(MainToolBar())
        self.setStatusBar(StatusBar())

        self._entityTreeView = QDockWidget()
        self._entityTreeView.setFeatures(QDockWidget.DockWidgetMovable | QDockWidget.DockWidgetFloatable)
        self._entityTreeView.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        self._entityTreeView.setWindowTitle("Entity Tree")
        self._entityTreeView.setWidget(EntityTreeView())
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._entityTreeView)

        self._entityFileView = QDockWidget()
        self._entityFileView.setFeatures(QDockWidget.DockWidgetMovable | QDockWidget.DockWidgetFloatable)
        self._entityFileView.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        self._entityFileView.setWindowTitle("Assets Explorer")
        self._entityFileView.setWidget(EntityFileView())
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._entityFileView)

        self._entityLogicsView = QDockWidget()
        self._entityLogicsView.setFeatures(QDockWidget.DockWidgetMovable | QDockWidget.DockWidgetFloatable)
        self._entityLogicsView.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        self._entityLogicsView.setWindowTitle("Entity Logics")
        self._entityLogicsView.setWidget(EntityLogicsView())
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, self._entityLogicsView)

        self._engineOutputView = EngineOutputView()

        self.setCentralWidget(self._engineOutputView)

        if not self._init():
            sys.exit(1)

        CreateEventManager(self)
        CreateEngineViewManager(self)

    def __del__(self):
        self._deinit()

    def _init(self):
        self._appConfig = AppConfig()
        self._editorNative = EditorNative(self._appConfig)
        if not self._editorNative.init():
            Log.error("[EditorView:_init] Can't init native editor")
            return False
        self._assetsModel = AssetsModel(self._appConfig)
        if not self._assetsModel.init():
            Log.error("[EditorView:_init] Can't init assets model")
            return False
        self._logicsModel = LogicsModel(self._editorNative)
        if not self._logicsModel.init():
            Log.error("[EditorView:_init] Can't init logics model")
            return False
        return True

    def _deinit(self):
        if self._editorNative is not None:
            self._editorNative.deinit()

def main():
    app = QApplication([])
    editor = EditorView()
    editor.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()