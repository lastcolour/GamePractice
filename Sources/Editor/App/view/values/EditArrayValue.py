from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton, QTreeWidgetItem, QLabel

class ArrayTitle(QWidget):
    def __init__(self, arrayEdit):
        super().__init__()

        self._arrayEdit = arrayEdit

        self._rootLayout = QHBoxLayout()
        self._rootLayout.setContentsMargins(1, 1, 1, 1)

        self._elemCountText = QLabel()
        self._rootLayout.addWidget(self._elemCountText)

        self._insertBt = QPushButton("add")
        self._insertBt.clicked.connect(self._signal_insertBt_clicked)
        self._rootLayout.addWidget(self._insertBt)

        self.setLayout(self._rootLayout)
        self._updateElemCountText()

    def _updateElemCountText(self):
        self._elemCountText.setText("{0} elements".format(len(self._arrayEdit._val.getValues())))

    def _signal_insertBt_clicked(self):
        self._arrayEdit._insertNewItem()

class EditArrayValue(QWidget):
    def __init__(self, value):
        self._val = value
        self._rootArrayItem = None
        self._logicView = None
        self._title = None

    def createTitle(self):
        self._title = ArrayTitle(self)
        return self._title

    def createObjecValueWrap(self, value):
        removeBt = QPushButton("x")
        removeBt.clicked.connect(lambda: self._signal_removeBt_clicked(removeBt))
        removeBt._value = value
        return removeBt

    def createSimpleWrap(self, editWidget):
        removeBt = QPushButton("x")
        removeBt.clicked.connect(lambda: self._signal_removeBt_clicked(removeBt))
        removeBt._value = editWidget._val
        editWidget._rootLayout.addWidget(removeBt)
        return editWidget

    def _insertNewItem(self):
        self._val.addNewElement()
        self._rebuildArrayView()

    def _signal_removeBt_clicked(self, button):
        self._val.removeElement(button._value)
        self._rebuildArrayView()

    def _rebuildArrayView(self):
        self._title._updateElemCountText()
        self._logicView._buildArrayTree(self._logicView._tree, self._rootArrayItem, self, self._val.getValues())
        self._logicView._updateTreeSize()
