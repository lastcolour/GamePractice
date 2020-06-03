from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton, QTreeWidgetItem

class ArrayTitle(QWidget):
    def __init__(self):
        super().__init__()

        self._arrayEdit = None

        self._rootLayout = QHBoxLayout()

        self._insertBt = QPushButton("add")
        self._insertBt.clicked.connect(self._signal_insertBt_clicked)
        self._rootLayout.addWidget(self._insertBt)

        self.setLayout(self._rootLayout)

    def _signal_insertBt_clicked(self):
        self._arrayEdit._insertNewItem()

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
        title = ArrayTitle()
        title._arrayEdit = self
        return title

    def createObjecTitle(self, value):
        return ArrayObjectTitle()

    def createSimpleWrap(self, editWidget):
        removeBt = QPushButton("x")
        editWidget._rootLayout.addWidget(removeBt)
        return editWidget

    def _insertNewItem(self):
        val = self._val.appendVal()
        editVal = self._logicView._createEditWidget(val)
        self._logicView._buildArrayTree(self._logicView._tree, self._rootArrayItem, self, [editVal, ])
        self._rootArrayItem.setExpanded(True)
        self._logicView._updateTreeSize()