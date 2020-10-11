from PyQt5.QtWidgets import QDialog, QTreeWidget, QTreeWidgetItem, QLineEdit, QVBoxLayout, QPushButton, QHBoxLayout, QStyle

from utils.ViewUtils import FilterTreeBySearchText

class LogicSelectDialog(QDialog):
    def __init__(self, logicsModel):
        super().__init__()

        self._resultLogic = None
        self._currentSelection = None

        self._rootLayout = QVBoxLayout()

        self._searchLine = QLineEdit()
        self._searchLine.setPlaceholderText("Search...")
        self._searchLine.setClearButtonEnabled(True)
        self._searchLine.textChanged.connect(self._signal_searchLine_textChanged)
        self._rootLayout.addWidget(self._searchLine)

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._tree.setSortingEnabled(False)
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

    def _buildTree(self, rootItem, moduleLogics):
        for module in moduleLogics:
            moduleItem = QTreeWidgetItem(rootItem)
            moduleItem.setText(0, module.getName())
            moduleItem.setIcon(0, self.style().standardIcon(QStyle.SP_DirIcon))
            for logic in module.getLogics():
                item = QTreeWidgetItem(moduleItem)
                item.setText(0, logic.getName())
                item._node = logic

    def _signal_cancelBt_clicked(self):
        self._resultLogic = None
        self.done(0)

    def reject(self):
        self._resultLogic = None
        super().reject()

    def _signal_addBt_clicked(self):
        self._resultLogic = self._currentSelection
        self.done(0)

    def _signal_searchLine_textChanged(self, text):
        FilterTreeBySearchText(self._tree, text)

    def getResultLogic(self):
        return self._resultLogic

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            if hasattr(currItem, "_node"):
                self._currentSelection = currItem._node
                self._addBt.setEnabled(True)
                return
        self._currentSelection = None
        self._addBt.setEnabled(False)