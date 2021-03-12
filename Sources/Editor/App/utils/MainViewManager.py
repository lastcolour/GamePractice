class MainViewManager:

    def __init__(self, mgr):
        self._mgr = mgr
        self._app = mgr._app

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