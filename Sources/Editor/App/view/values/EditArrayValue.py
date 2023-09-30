from PyQt6.QtWidgets import QWidget, QHBoxLayout, QPushButton, QLabel

from view.values.UIUitls import FreezeEditButtonSize

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
    def __init__(self, value, treeItem, logicView):
        self._val = value
        self._rootArrayItem = treeItem
        self._logicView = logicView
        self._title = ArrayTitle(self)
        self._prevElemCount = 0

    def createTitle(self):
        return self._title

    def createObjecValueWrap(self, value):
        wrapWidget = QWidget()
        wrapWidget._rootLayout = QHBoxLayout()
        wrapWidget._rootLayout.setContentsMargins(0, 0, 0, 0)

        removeBt = QPushButton("x")
        FreezeEditButtonSize(removeBt)
        removeBt.clicked.connect(lambda: self._signal_removeBt_clicked(removeBt))
        removeBt._value = value

        wrapWidget._rootLayout.addStretch()
        wrapWidget._rootLayout.addWidget(removeBt)

        wrapWidget.setLayout(wrapWidget._rootLayout)

        return wrapWidget

    def createSimpleWrap(self, editWidget):
        removeBt = QPushButton("x")
        FreezeEditButtonSize(removeBt)
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