from PyQt5.QtWidgets import QDialog, QHBoxLayout, QVBoxLayout, QPushButton, QTreeWidget, QTreeWidgetItem

class SelectFromEntityChild(QDialog):
    def __init__(self, entity):
        super().__init__()

        self._entity = entity
        self._selectedEntity = None

        self._rootLayout = QVBoxLayout()
        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._tree.setSortingEnabled(False)
        self._tree.currentItemChanged.connect(self._signal_tree_currentItemChanged)
        self._buildTree(self._entity, self._tree.invisibleRootItem())
        self._rootLayout.addWidget(self._tree)

        self._buttotLayout = QHBoxLayout()

        self._cancelBt = QPushButton("Cancel")
        self._cancelBt.clicked.connect(self._signal_cancelBt_clicked)
        self._buttotLayout.addWidget(self._cancelBt)

        self._selectBt = QPushButton("Select")
        self._selectBt.clicked.connect(self._signal_selectBt_clicked)
        self._selectBt.setEnabled(False)
        self._buttotLayout.addWidget(self._selectBt)

        self._rootLayout.addLayout(self._buttotLayout)

        self.setLayout(self._rootLayout)
        self.setWindowTitle("Select Entity")

    def _signal_cancelBt_clicked(self):
        self._selectedEntity = None
        self.done(0)

    def _signal_selectBt_clicked(self):
        self.done(0)

    def _buildTree(self, rootEntity, rootItem):
        item = QTreeWidgetItem(rootItem)
        if  rootEntity == self._entity:
            item.setText(0, "{0} (this)".format(rootEntity.getName()))
        else:
            if rootEntity.isInternal():
                item.setText(0, rootEntity.getName())
            else:
                item.setText(0, "{0} {1}".format(rootEntity.getName(), rootEntity.getNameSuffix()))
        item._entity = rootEntity
        for childEnt in rootEntity.getChildren():
            self._buildTree(childEnt, item)

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            self._selectedEntity = currItem._entity
            self._selectBt.setEnabled(True)
        else:
            self._selectedEntity = None
            self._selectBt.setEnabled(False)

    def getSelectedEntity(self):
        return self._selectedEntity

    def reject(self):
        self._selectedEntity = None
        super().reject()