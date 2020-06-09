from PyQt5.QtWidgets import QDialog, QHBoxLayout, QVBoxLayout, QPushButton, QListWidget, QListWidgetItem

class SelectFromEntityChild(QDialog):
    def __init__(self, entity):
        super().__init__()

        self._entity = entity
        self._selectedEntity = None

        self._rootLayout = QVBoxLayout()
        self._listView = QListWidget()
        self._listView.currentItemChanged.connect(self._signal_listView_currentItemChanged)
        self._buildListView()
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
        self.setWindowTitle("Select Entity")

    def _signal_cancelBt_clicked(self):
        self._selectedEntity = None
        self.done(0)

    def _signal_selectBt_clicked(self):
        self.done(0)

    def _buildListView(self):
        for childEnt in self._entity.getChildren():
            item = QListWidgetItem(childEnt.getName())
            item._entity = childEnt
            self._listView.addItem(item)

    def _signal_listView_currentItemChanged(self, currItem, prevItem):
        if currItem is not None:
            self._selectedEntity = currItem._entity
            self._selectBt.setEnabled(True)
        else:
            self._selectedEntity = None
            self._selectBt.setEnabled(False)

    def getSelectedEntity(self):
        return self._selectedEntity