from PyQt5.QtWidgets import QWidget, QTreeWidget, QVBoxLayout, QTreeWidgetItem, QLineEdit, QStyle
from PyQt5 import QtCore

from menu.EntityTreeMenu import EntityTreeMenu
from utils.EventManager import GetEventManager

class EntityTreeView(QWidget):
    def __init__(self):
        super().__init__()

        self._editEntity = None

        self._rootLayout = QVBoxLayout()

        self._searchLine = QLineEdit()
        self._searchLine.setPlaceholderText("Search...")
        self._rootLayout.addWidget(self._searchLine)

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._tree.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self._tree.customContextMenuRequested.connect(self._signal_tree_contextMenuRequested)
        self._tree.currentItemChanged.connect(self._signal_tree_currentItemChanged)
        self._rootLayout.addWidget(self._tree)

        self._entityTreeMenu = EntityTreeMenu(self)

        self.setLayout(self._rootLayout)

    def setEditEntity(self, editEntity):
        self._editEntity = editEntity
        self._buildTree()

    def _createTreeItem(self, parentItem, entity):
        treeItem = QTreeWidgetItem(parentItem)
        treeItem.setText(0, entity.getName())
        treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_FileIcon))
        treeItem._entity = entity
        for child in entity.getChildren():
            self._createTreeItem(treeItem, child)

    def _buildTree(self):
        self._tree.clear()
        self._createTreeItem(self._tree.invisibleRootItem(), self._editEntity)

    def _onAddNewChild(self, treeItem, entityName):
        entity = treeItem._entity
        childEntity = entity.addChildEntity(entityName)
        if childEntity is None:
            return
        self._createTreeItem(treeItem, childEntity)

    def _onRemoveChild(self, treeItem):
        parentItem = treeItem.parent()
        parentEntity = treeItem.parent()._entity
        childEntity = treeItem._entity
        parentEntity.removeChildEntity(childEntity)
        parentItem.removeChild(treeItem)

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is None or not hasattr(currItem, "_entity"):
            GetEventManager().onEntityClickedFromEntityTree(None)
        else:
            GetEventManager().onEntityClickedFromEntityTree(currItem._entity)

    def _signal_tree_contextMenuRequested(self, pt):
        item = self._tree.itemAt(pt)
        globalPt = self._tree.viewport().mapToGlobal(pt)
        self._entityTreeMenu.onMenuRequestedOnItem(item, globalPt)