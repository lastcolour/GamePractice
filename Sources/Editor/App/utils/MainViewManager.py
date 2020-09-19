from PyQt5 import QtCore

class _MainViewManager:

    _INSTANCE = None

    def __init__(self, app):
        self._app = app

    def onEntityDoubleClickFromFileTree(self, editEntity):
        if self._app._entityLogicsView is not None:
            self._app._entityLogicsView.setEditEntity(editEntity)
        if self._app._entityTreeView is not None:
            self._app._entityTreeView.setEditEntity(editEntity)

    def onEntityClickedFromEntityTree(self, editEntity):
        if self._app._entityLogicsView is not None:
            self._app._entityLogicsView.setEditEntity(editEntity)

    def closeEditEntity(self):
        if self._app._entityLogicsView is not None:
            self._app._entityLogicsView.setEditEntity(None)
        if self._app._entityTreeView is not None:
            self._app._entityTreeView.setEditEntity(None)

    def openEntityTreeView(self):
        if self._app._entityTreeView is not None:
            return
        self._app._openEntityTreeView()

    def openFileTreeView(self):
        if self._app._entityFileView is not None:
            return
        self._app._openFileTreeView()

    def openEntityLogicsView(self):
        if self._app._entityLogicsView is not None:
            return
        self._app._openEntityLogicsView()

    def onMainViewClosed(self, view):
        if view == self._app._entityFileView:
            self._app._entityFileView = None
        elif view == self._app._entityLogicsView:
            self._app._entityLogicsView = None
        elif view == self._app._entityTreeView:
            self._app._entityTreeView = None
        else:
            raise RuntimeError("Unknown main view")

def GetMainViewManager():
    return _MainViewManager._INSTANCE

def CreateMainViewManager(app):
    manger = _MainViewManager(app)
    _MainViewManager._INSTANCE = manger