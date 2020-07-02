from PyQt5.QtWidgets import QDialog, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton

from view.base.FileTreeView import FileTreeView

from utils.EventManager import GetEventManager
from utils.ViewUtils import FilterTreeBySearchText

class SelectResourceFile(QDialog):
    def __init__(self):
        super().__init__()

        self._selectedFile = None

        self._rootLayout = QVBoxLayout()

        self._lineEdit = QLineEdit()
        self._lineEdit.setPlaceholderText("Search...")
        self._lineEdit.setClearButtonEnabled(True)
        self._lineEdit.textChanged.connect(self._signal_search_textChanged)
        self._rootLayout.addWidget(self._lineEdit)

        self._tree = FileTreeView()
        self._tree._setFileTreeModel(GetEventManager().getAssetsModel().getResourcesTree())
        self._tree.currentItemChanged.connect(self._signal_tree_currentItemChanged)
        self._rootLayout.addWidget(self._tree)

        self._buttonLayout = QHBoxLayout()

        self._cancelBt = QPushButton("Cancel")
        self._cancelBt.clicked.connect(self._signal_cancelBt_clicked)
        self._buttonLayout.addWidget(self._cancelBt)

        self._selectBt = QPushButton("Select")
        self._selectBt.clicked.connect(self._signal_selectBt_clicked)
        self._selectBt.setEnabled(False)
        self._buttonLayout.addWidget(self._selectBt)

        self._rootLayout.addLayout(self._buttonLayout)

        self.setLayout(self._rootLayout)
        self.setWindowTitle("Select Resource")

    def getSelectedResource(self):
        if self._selectedFile is None:
            return None
        return self._selectedFile.getRelativePath()

    def _signal_cancelBt_clicked(self):
        self._selectedFile = None
        self.done(0)

    def _signal_selectBt_clicked(self):
        self.done(0)

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            if not currItem._node.isDir():
                self._selectedFile = currItem._node
                self._selectBt.setEnabled(True)
            else:
                self._selectedFile = None
                self._selectBt.setEnabled(False)
        else:
            self._selectedFile = None
            self._selectBt.setEnabled(False)

    def _signal_search_textChanged(self, text):
        FilterTreeBySearchText(self._tree, text)