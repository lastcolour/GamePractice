from PyQt5.QtWidgets import QDialog, QTreeWidget, QTreeWidgetItem, QLineEdit, QVBoxLayout, QPushButton, QHBoxLayout

class LogicSelectDialog(QDialog):
    def __init__(self, logicsModel):
        super().__init__()

        self._resultLogic = None

        self._rootLayout = QVBoxLayout()

        self._searchLine = QLineEdit()
        self._searchLine.setPlaceholderText("Search...")
        self._rootLayout.addWidget(self._searchLine)

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._tree.currentItemChanged.connect(self._signal_tree_currentItemChanged)
        self._buildTree(self._tree.invisibleRootItem(), logicsModel.getLogics())
        self._tree.setSortingEnabled(True)
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
        self.setWindowTitle("Select Entity Logic")

    def _buildTree(self, rootItem, logics):
        for logic in logics:
            item = QTreeWidgetItem(rootItem)
            item.setText(0, logic.getName())
            item._node = logic

    def _signal_cancelBt_clicked(self):
        self._resultLogic = None
        self.done(0)

    def _signal_addBt_clicked(self):
        self.done(0)

    def getResultLogic(self):
        return self._resultLogic

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            self._resultLogic = currItem._node
            self._addBt.setEnabled(True)
        else:
            self._resultLogic = None
            self._addBt.setEnabled(False)