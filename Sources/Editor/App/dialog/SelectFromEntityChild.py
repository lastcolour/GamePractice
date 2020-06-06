from PyQt5.QtWidgets import QDialog, QHBoxLayout, QVBoxLayout, QPushButton, QListWidget

class SelectFromEntityChild(QDialog):
    def __init__(self, entity):
        super().__init__()

        self._entity = entity 

        self._rootLayout = QVBoxLayout()
        self._listView = QListWidget()
        self._listView.currentItemChanged.connect(self._)
        self._rootLayout.addWidget(self._listView)

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

    def _signal_cancelBt_clicked(self):
        self.done(0)

    def _signal_selectBt_clicked(self):
        self.done(0)

    def _signal_listView_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            if not currItem._node.isDir():
                self._resultEntityFileNode = currItem._node
                self._addBt.setEnabled(True)
                return
        self._addBt.setEnabled(False)