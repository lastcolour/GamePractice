from PyQt5.QtWidgets import QDialog, QTreeWidgetItem, QTreeWidget, QHBoxLayout, QVBoxLayout, QPushButton, QLineEdit

from view.base.FileTreeView import FileTreeView
from utils.EventManager import GetEventManager

def _disableItemIfCantBeChild(entity, treeItem):
    for i in range(treeItem.childCount()):
        childItem = treeItem.child(i)
        if not childItem._node.isDir():
            path = childItem._node.getRelativePath()
            if not entity.canAddChild(path):
                childItem.setDisabled(True)
        else:
            _disableItemIfCantBeChild(entity, childItem)

class SelectChildEntity(QDialog):
    def __init__(self, entity):
        super().__init__()

        self._entity = entity
        self._resultEntityFileNode = None

        self._rootLayout = QVBoxLayout()

        self._searchLine = QLineEdit()
        self._searchLine.setPlaceholderText("Search...")
        self._rootLayout.addWidget(self._searchLine)

        self._tree = FileTreeView()
        self._tree._setFileTreeModel(GetEventManager().getAssetsModel())
        _disableItemIfCantBeChild(self._entity, self._tree.invisibleRootItem())
        self._tree.currentItemChanged.connect(self._signal_tree_currentItemChanged)
        self._rootLayout.addWidget(self._tree)

        self._buttotLayout = QHBoxLayout()

        self._cancelBt = QPushButton("Cancel")
        self._cancelBt.clicked.connect(self._signal_cancelBt_clicked)
        self._buttotLayout.addWidget(self._cancelBt)

        self._addBt = QPushButton("Add")
        self._addBt.clicked.connect(self._signal_addBt_clicked)
        self._addBt.setEnabled(False)
        self._buttotLayout.addWidget(self._addBt)

        self._rootLayout.addLayout(self._buttotLayout)

        self.setLayout(self._rootLayout)
        self.setWindowTitle("Add Child Entity")

    def getResultEntity(self):
        return self._resultEntityFileNode

    def _signal_cancelBt_clicked(self):
        self._resultEntityFileNode = None
        self.done(0)

    def _signal_addBt_clicked(self):
        self.done(0)

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            if not currItem._node.isDir():
                self._resultEntityFileNode = currItem._node
                self._addBt.setEnabled(True)
                return
        self._resultEntityFileNode = None
        self._addBt.setEnabled(False)