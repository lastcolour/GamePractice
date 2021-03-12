from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton, QTreeWidgetItem, QLabel

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

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

    def _pull(self):
        self._arrayEdit._rebuildView()

class EditArrayValue(QWidget):
    def __init__(self, value):
        self._val = value
        self._rootArrayItem = None
        self._logicView = None
        self._title = None
        self._prevElemCount = 0

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
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _signal_removeBt_clicked(self, button):
        self._val.removeElement(button._value)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _rebuildView(self):
        if len(self._val.getValues()) == self._prevElemCount:
            return
        
        self._prevElemCount = len(self._val.getValues())
        self._title._updateElemCountText()
        self._logicView._buildArrayTree(self._logicView._tree, self._rootArrayItem, self, self._val.getValues())
        self._logicView._updateTreeSize()