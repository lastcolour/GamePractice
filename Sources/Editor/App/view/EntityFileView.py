from PyQt6.QtWidgets import QWidget, QTreeWidget, QLineEdit, QVBoxLayout, QTreeWidgetItem, QStyle
from PyQt6 import QtCore

from menu.FileTreeMenu import FileTreeMenu
from utils.Managers import GetEventManager
from utils.ViewUtils import FilterTreeBySearchText
from view.base.FileTreeView import FileTreeView

from msg.Messages import MsgOpenEntityForEdit
from msg.MessageSystem import SendMessage

class EntityFileView(QWidget):
    def __init__(self):
        super().__init__()
        self._fileTreeModel = None

        self._rootLayout = QVBoxLayout()

        self._search = QLineEdit()
        self._search.setPlaceholderText("Search...")
        self._search.setClearButtonEnabled(True)
        self._search.textChanged.connect(self._signal_search_textChanged)
        self._rootLayout.addWidget(self._search)

        self._tree = FileTreeView()
        self._tree.setContextMenuPolicy(QtCore.Qt.ContextMenuPolicy.CustomContextMenu)
        self._tree.customContextMenuRequested.connect(self._signal_tree_contextMenuRequested)
        self._tree.itemDoubleClicked.connect(self._signal_tree_itemDoublecClicked)
        self._rootLayout.addWidget(self._tree)

        self._fileTreeMenu = FileTreeMenu(self)

        self.setLayout(self._rootLayout)

    def _signal_search_textChanged(self, text):
        FilterTreeBySearchText(self._tree, text)

    def _signal_tree_contextMenuRequested(self, pt):
        item = self._tree.itemAt(pt)
        globalPt = self._tree.viewport().mapToGlobal(pt)
        self._fileTreeMenu.onMenuRequestedOnItem(item, globalPt)

    def setFileTreeModel(self, fileTreeModel):
        self._fileTreeModel = fileTreeModel
        self._tree._setFileTreeModel(self._fileTreeModel)

    def _signal_tree_itemDoublecClicked(self, item):
        if item._node.isDir():
            return
        entityName = item._node.getRelativePath()
        SendMessage(MsgOpenEntityForEdit(entityName))

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
        return self._fileTreeModel

    def _createNewDir(self, treeItem, fileName):
        assetModel = GetEventManager().getAssetsModel()
        newNode = assetModel.createNewDir(self._getRootFileNode(treeItem), fileName)
        if newNode is None:
            return False
        treeItem._node = newNode
        treeItem.setText(0, fileName)
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _renameItem(self, treeItem, newFileName):
        assetModel = GetEventManager().getAssetsModel()
        if not assetModel.renameNode(treeItem._node, newFileName):
            return False
        treeItem.setText(0, newFileName)
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _createNewEntity(self, treeItem, fileName):
        assetModel = GetEventManager().getAssetsModel()
        newNode = assetModel.createNewEntity(self._getRootFileNode(treeItem), fileName)
        if newNode is None:
            return False
        treeItem._node = newNode
        treeItem.setText(0, fileName)
        self._tree.setItemWidget(treeItem, 0, None)
        return True

    def _removeItem(self, treeItem):
        assetModel = GetEventManager().getAssetsModel()
        if not assetModel.removeNode(treeItem._node):
            return
        parentItem = treeItem.parent()
        if parentItem is None:
            parentItem = self._tree.invisibleRootItem()
        parentItem.removeChild(treeItem)

    def _createEditDirItem(self, treeItem):
        newItem = QTreeWidgetItem(self._getRootItemForEditItem(treeItem))
        newItem.setIcon(0, self.style().standardIcon(QStyle.StandardPixmap.SP_DirIcon))
        return newItem

    def _createEditFileItem(self, treeItem):
        newItem = QTreeWidgetItem(self._getRootItemForEditItem(treeItem))
        newItem.setIcon(0, self.style().standardIcon(QStyle.StandardPixmap.SP_FileIcon))
        return newItem