from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6 import QtCore

from utils.Log import Log
from utils.AppConfig import AppConfig
from utils.Managers import CreateManagers

from view.EntityFileView import EntityFileView
from view.EntityLogicsView import EntityLogicsView
from view.EntityTreeView import EntityTreeView
from view.base.MainDockWidget import WrapMainDockWidget
from view.EngineOutputView import EngineOutputView

from native.EditorNative import EditorNative

from model.AssetsModel import AssetsModel
from model.LogicsModel import LogicsModel
from model.SoundEventsModel import SoundEventsModel

from menu.MainFileMenu import MainFileMenu
from menu.MainToolBar import MainToolBar
from menu.MainViewMenu import MainViewMenu

from view.main.StatusBar import StatusBar

import sys
import os

class EditorView(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Editor")
        self.setMinimumSize(640, 480)
        self.setContentsMargins(1, 1, 1, 1)

        self._fileMenu = MainFileMenu(self)
        self._viewMenu = MainViewMenu(self)

        self.menuBar().addMenu(self._fileMenu)
        self.menuBar().addMenu(self._viewMenu)

        self.addToolBar(MainToolBar())
        self.setStatusBar(StatusBar())

        self._engineOutputView = EngineOutputView()
        self.setCentralWidget(self._engineOutputView)

        if not self._init():
            sys.exit(1)

        CreateManagers(self)

        self._openEntityTreeView()
        self._openFileTreeView()
        self._openEntityLogicsView()

    def _openEntityTreeView(self):
        self._entityTreeView = EntityTreeView()
        self.addDockWidget(QtCore.Qt.DockWidgetArea.LeftDockWidgetArea, WrapMainDockWidget(self._entityTreeView, "Entity Tree"))

    def _openFileTreeView(self):
        self._entityFileView = EntityFileView()
        self._entityFileView.setFileTreeModel(self._assetsModel.getEntitiesTree())
        self.addDockWidget(QtCore.Qt.DockWidgetArea.LeftDockWidgetArea, WrapMainDockWidget(self._entityFileView, "Assets Explorer"))

    def _openEntityLogicsView(self):
        self._entityLogicsView = EntityLogicsView()
        self.addDockWidget(QtCore.Qt.DockWidgetArea.RightDockWidgetArea, WrapMainDockWidget(self._entityLogicsView, "Entity Logics"))

    def closeEvent(self, event):
        self._deinit()
        return super().closeEvent(event)

    def __del__(self):
        self._deinit()

    def _init(self):
        self._appConfig = AppConfig()
        self._editorNative = EditorNative(self._appConfig)
        if not self._editorNative.init():
            self._editorNative = None
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
        self._soundEventsModel = SoundEventsModel(self._appConfig)
        if not self._soundEventsModel.init():
            Log.error("[EditorView:_init] Can't init sound events model")
            return False
        return True

    def _deinit(self):
        if self._editorNative is not None:
            self._editorNative.deinit()
            self._editorNative = None

def main():
    app = QApplication([])
    app.setStyle("windows")
    editor = EditorView()
    editor.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()