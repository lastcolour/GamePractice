from PyQt5.QtWidgets import QTreeWidget, QTreeWidgetItem, QStyle

class FileTreeView(QTreeWidget):
    def __init__(self):
        super().__init__()
        self._fileTreeModel = None

        self.setHeaderHidden(True)
        self.setSortingEnabled(False)
        self.setColumnCount(1)

    def _setFileTreeModel(self, model):
        self._fileTreeModel = model

    def _buildTree(self, parentItem, rootNode):
        for node in rootNode:
            if node.isDir():
                treeItem = QTreeWidgetItem(parentItem)
                treeItem.setText(0, node.getBaseName())
                treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_DirIcon))
                treeItem._node = node
                self._buildTree(treeItem, node)
            else:
                treeItem = QTreeWidgetItem(parentItem)
                treeItem.setText(0, node.getBaseName())
                treeItem.setIcon(0, self.style().standardIcon(QStyle.SP_FileIcon))
                treeItem._node = node

    def _setFileTreeModel(self, fileTreeModel):
        self.clear()
        self._fileTreeModel = fileTreeModel
        if self._fileTreeModel is not None:
            self._buildTree(self.invisibleRootItem(), self._fileTreeModel.getEntitiesTree())