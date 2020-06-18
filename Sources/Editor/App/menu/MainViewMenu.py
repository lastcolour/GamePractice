from PyQt5.QtWidgets import QMenu, QAction

from utils.MainViewManager import GetMainViewManager

class MainViewMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)

        self._openEntityTreeAct = QAction("Entity Tree View")
        self._openEntityTreeAct.triggered.connect(self._onOpenEntityTree)

        self._openEntityLogicsAct = QAction("Entity Logics")
        self._openEntityLogicsAct.triggered.connect(self._onOpenEntityLogics)

        self._openAssetsExplorerAct = QAction("Assets Explorer")
        self._openAssetsExplorerAct.triggered.connect(self._onOpenAssetsExplorer)

        self.addAction(self._openEntityTreeAct)
        self.addAction(self._openEntityLogicsAct)
        self.addAction(self._openAssetsExplorerAct)
        self.setTitle("View")

    def _onOpenEntityTree(self):
        GetMainViewManager().openEntityTreeView()

    def _onOpenEntityLogics(self):
        GetMainViewManager().openEntityLogicsView()

    def _onOpenAssetsExplorer(self):
        GetMainViewManager().openFileTreeView()