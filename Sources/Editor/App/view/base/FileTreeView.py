from PyQt5.QtWidgets import QTreeWidget, QTreeWidgetItem, QStyle

class FileTreeView(QTreeWidget):
    def __init__(self):
        super().__init__()
        self._fileTreeModel = None
        self._expandedItems = {}

        self.setHeaderHidden(True)
        self.setSortingEnabled(False)
        self.setColumnCount(1)

        self.itemExpanded.connect(self._on_itemExpanded)
        self.itemCollapsed.connect(self._on_itemCollapsed)

    def _buildTree(self, parentItem, rootNode):
        for node in rootNode:
            if node.isDir():
                treeItem = QTreeWidgetItem(parentItem)
                treeItem.setText(0, node.getBaseName())
                treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_DirIcon))
                treeItem._node = node
                if node.getRelativePath() in self._expandedItems:
                    treeItem.setExpanded(True)
                self._buildTree(treeItem, node)
            else:
                treeItem = QTreeWidgetItem(parentItem)
                treeItem.setText(0, node.getBaseName())
                treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_FileIcon))
                treeItem._node = node

    def _on_itemExpanded(self, item):
        self._expandedItems[item._node.getRelativePath()] = True

    def _on_itemCollapsed(self, item):
        del self._expandedItems[item._node.getRelativePath()]

    def _setFileTreeModel(self, fileTreeModel):
        self.clear()
        self._fileTreeModel = fileTreeModel
        if self._fileTreeModel is not None:
            self._buildTree(self.invisibleRootItem(), self._fileTreeModel)