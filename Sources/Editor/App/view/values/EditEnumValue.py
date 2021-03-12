from PyQt5.QtWidgets import QWidget, QHBoxLayout

from view.values.EditWidgets import EditComboBox

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditEnumValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()
        self._rootLayout.addStretch()

        self._comboBox = EditComboBox()
        enumItems = [x for x in self._val.getTable()]
        enumItems.sort()
        for item in enumItems:
            self._comboBox.addItem(item)
        self._comboBox.currentIndexChanged.connect(self._signal_comboBox_indexChanged)
        self._rootLayout.addWidget(self._comboBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_comboBox_indexChanged(self, idx):
        text = self._comboBox.currentText()
        self._push(text)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, text):
        self._val.setVal(text)

    def _pull(self):
        idx = self._comboBox.findText(self._val.getVal())
        if idx == -1:
            raise RuntimeError("Can't find value '{0}' in combo box".format(self._val.getVal()))
        if idx != self._comboBox.currentIndex():
            self._comboBox.setCurrentIndex(idx)