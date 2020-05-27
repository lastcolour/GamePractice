from PyQt5.QtWidgets import QWidget, QTreeWidget, QLineEdit, QVBoxLayout, QTreeWidgetItem, QStyle
from PyQt5 import QtCore

from menu.FileTreeMenu import FileTreeMenu 

class EntityFileView(QWidget):
    def __init__(self):
        super().__init__()
        self._fileTreeModel = None

        self._rootLayout = QVBoxLayout()

        self._search = QLineEdit()
        self._search.setPlaceholderText("Search...")
        self._rootLayout.addWidget(self._search)

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._tree.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self._tree.customContextMenuRequested.connect(self._signal_tree_contextMenuRequested)
        self._rootLayout.addWidget(self._tree)

        self._fileTreeMenu = FileTreeMenu(self._tree)

        self.setLayout(self._rootLayout)

    def _signal_tree_contextMenuRequested(self, pt):
        item = self._tree.itemAt(pt)
        globalPt = self._tree.viewport().mapToGlobal(pt)
        self._fileTreeMenu.onMenuRequestedOnItem(item, globalPt)

    def _buildTree(self, rootItem, rootNode):
        for node in rootNode:
            if node.isDir():
                treeItem = QTreeWidgetItem(rootItem)
                treeItem.setText(0, node.getBaseName())
                treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_DirIcon))
                treeItem._node = node
                self._buildTree(treeItem, node)
            else:
                treeItem = QTreeWidgetItem(rootItem)
                treeItem.setText(0, node.getBaseName())
                treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_FileIcon))
                treeItem._node = node

    def setFileTreeModel(self, fileTreeModel):
        self._tree.clear()
        self._fileTreeModel = fileTreeModel
        if self._fileTreeModel is not None:
            self._buildTree(self._tree.invisibleRootItem(), self._fileTreeModel.getEntitiesTree())
