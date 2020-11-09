from PyQt5.QtWidgets import QWidget, QTreeWidget, QVBoxLayout, QTreeWidgetItem, QLineEdit, QStyle
from PyQt5 import QtCore

from menu.EntityTreeMenu import EntityTreeMenu
from utils.Managers import GetEventManager
from utils.ViewUtils import FilterTreeBySearchText

class EntityTreeView(QWidget):
    def __init__(self):
        super().__init__()

        self._editEntity = None

        self._rootLayout = QVBoxLayout()

        self._searchLine = QLineEdit()
        self._searchLine.setPlaceholderText("Search...")
        self._searchLine.setClearButtonEnabled(True)
        self._searchLine.textChanged.connect(self._signal_searchLine_textChanged)
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
        if self._editEntity is not None:
            self._createTreeItem(self._tree.invisibleRootItem(), self._editEntity)

    def _onAddNewChild(self, treeItem, entityName):
        entity = treeItem._entity
        childEntity = entity.addChildEntity(entityName)
        if childEntity is None:
            return
        self._createTreeItem(treeItem, childEntity)
        GetEventManager().onEntityClickedFromEntityTree(childEntity)

    def _onRemoveChild(self, treeItem):
        parentItem = treeItem.parent()
        parentEntity = treeItem.parent()._entity
        childEntity = treeItem._entity
        parentEntity.removeChildEntity(childEntity)
        parentItem.removeChild(treeItem)

    def _signal_searchLine_textChanged(self, text):
        FilterTreeBySearchText(self._tree, text)

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None and hasattr(currItem, "_entity"):
            GetEventManager().onEntityClickedFromEntityTree(currItem._entity)

    def _signal_tree_contextMenuRequested(self, pt):
        item = self._tree.itemAt(pt)
        globalPt = self._tree.viewport().mapToGlobal(pt)
        self._entityTreeMenu.onMenuRequestedOnItem(item, globalPt)

    def _createEditItem(self, treeItem):
        newItem = QTreeWidgetItem(treeItem)
        newItem.setIcon(0, self.style().standardIcon(QStyle.SP_FileIcon))
        return newItem

    def _createNewChildEntity(self, treeItem, childName):
        parentItem = treeItem.parent()
        newEntity = parentItem._entity.createNewInternalChild(childName)
        if newEntity is None:
            return False
        treeItem._entity = newEntity
        treeItem.setText(0, newEntity.getName())
        self._tree.setItemWidget(treeItem, 0, None)
        GetEventManager().onEntityClickedFromEntityTree(treeItem._entity)
        return True

    def _renameChildEntity(self, treeItem, childName):
        if not treeItem._entity.rename(childName):
            return False
        treeItem.setText(0, treeItem._entity.getName())
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _extractEntityToFile(self, treeItem):
        if not GetEventManager().onChildEntityExtractToFile(treeItem._entity):
            return
        treeItem.setText(0, treeItem._entity.getName())