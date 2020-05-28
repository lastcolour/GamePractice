from PyQt5.QtWidgets import QMenu, QAction

class FileTreeMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)

        self._copyItem = None
        self._cutItem = None

        self._newDirectoryAct = QAction("New Directoty")
        self._newEntityAct = QAction("New Entity")
        self._copyRelativePathAct = QAction("Copy Relative Path")
        self._renameAct = QAction("Rename")
        self._copyAct = QAction("Copy")
        self._cutAct = QAction("Cut")
        self._pasteAct = QAction("Paste")
        self._removeAct = QAction("Remove")
        self._showInExplorerAct = QAction("Show in Explorer")

        self.addAction(self._newDirectoryAct)
        self.addAction(self._newEntityAct)
        self.addAction(self._copyRelativePathAct)
        self.addAction(self._renameAct)
        self.addAction(self._copyAct)
        self.addAction(self._cutAct)
        self.addAction(self._pasteAct)
        self.addAction(self._removeAct)
        self.addAction(self._showInExplorerAct)

    def onMenuRequestedOnItem(self, item, pt):
        if item is None:
            self._copyAct.setEnabled(False)
            self._cutAct.setEnabled(False)
            self._removeAct.setEnabled(False)
            self._renameAct.setEnabled(False)
            self._copyRelativePathAct.setEnabled(False)
        else:
            self._copyAct.setEnabled(True)
            self._cutAct.setEnabled(True)
            self._removeAct.setEnabled(True)
            self._renameAct.setEnabled(True)
            self._copyRelativePathAct.setEnabled(True)
        if self._copyItem is None and self._cutItem is None:
            self._pasteAct.setEnabled(False)
        else:
            self._pasteAct.setEnabled(True)
        self.exec(pt)