from PyQt5.QtWidgets import QWidget, QTreeWidget, QLineEdit, QVBoxLayout, QTreeWidgetItem, QStyle
from PyQt5 import QtCore

from menu.FileTreeMenu import FileTreeMenu 
from utils.EventManager import GetEventManager

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
        self._tree.itemDoubleClicked.connect(self._tree_itemDoublecClicked)
        self._rootLayout.addWidget(self._tree)

        self._fileTreeMenu = FileTreeMenu(self)

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

    def _tree_itemDoublecClicked(self, item):
        if item._node.isDir():
            return
        GetEventManager().onEntityDoubleClickFromFileTree(item._node.getRelativePath())

    def _getRootItemForEditItem(self, treeItem):
        if treeItem is None:
            treeItem = self._tree.invisibleRootItem()
        elif not treeItem._node.isDir():
            treeItem = treeItem.parent()
            if treeItem is None:
                treeItem = self._tree.invisibleRootItem()
        return treeItem

    def _getRootFileNode(self, treeItem):
        if hasattr(treeItem.parent(), "_node"):
            return treeItem.parent()._node
        return self._fileTreeModel.getRootNode()

    def _createNewDir(self, treeItem, fileName):
        newNode = self._fileTreeModel.createNewDir(self._getRootFileNode(treeItem), fileName)
        if newNode is None:
            return False
        treeItem._node = newNode
        treeItem.setText(0, fileName)
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _renameItem(self, treeItem, newFileName):
        if not self._fileTreeModel.renameNode(treeItem._node, newFileName):
            return False
        treeItem.setText(0, newFileName)
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _createNewEntity(self, treeItem, fileName):
        newNode = self._fileTreeModel.createNewEntity(self._getRootFileNode(treeItem), fileName)
        if newNode is None:
            return False
        treeItem._node = newNode
        treeItem.setText(0, fileName)
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _removeItem(self, treeItem):
        if not self._fileTreeModel.removeNode(treeItem._node):
            return
        parentItem = treeItem.parent()
        if parentItem is None:
            parentItem = self._tree.invisibleRootItem()
        parentItem.removeChild(treeItem)

    def _createEditDirItem(self, treeItem):
        newItem = QTreeWidgetItem(self._getRootItemForEditItem(treeItem))
        newItem.setIcon(0, self.style().standardIcon(QStyle.SP_DirIcon))
        return newItem

    def _createEditFileItem(self, treeItem):
        newItem = QTreeWidgetItem(self._getRootItemForEditItem(treeItem))
        newItem.setIcon(0, self.style().standardIcon(QStyle.SP_FileIcon))
        return newItem