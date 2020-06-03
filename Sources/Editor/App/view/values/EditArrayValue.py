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
        self._updateElemCountText()

class ArrayObjectTitle(QWidget):
    def __init__(self):
        super().__init__()

        self._rootLayout = QHBoxLayout()

        self._clearBt = QPushButton("x")
        self._rootLayout.addWidget(self._clearBt)

        self.setLayout(self._rootLayout)

class ArrayElement:
    def __init__(self):
        pass

    def setVal(self):
        pass

    def getVal(self):
        pass

class EditArrayValue(QWidget):
    def __init__(self, value):
        self._val = value
        self._rootArrayItem = None
        self._logicView = None

    def createTitle(self):
        title = ArrayTitle(self)
        return title

    def createObjecTitle(self, value):
        return ArrayObjectTitle()

    def createSimpleWrap(self, editWidget):
        removeBt = QPushButton("x")
        editWidget._rootLayout.addWidget(removeBt)
        return editWidget

    def _insertNewItem(self):
        self._logicView._buildArrayTree(self._logicView._tree, self._rootArrayItem, self, [self._val.addNewElement(), ])
        self._rootArrayItem.setExpanded(True)
        self._logicView._updateTreeSize()