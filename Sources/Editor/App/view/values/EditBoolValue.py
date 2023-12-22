from PyQt6.QtWidgets import QCheckBox, QWidget, QHBoxLayout
from PyQt6.QtCore import Qt

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditBoolValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()
        self._rootLayout.addStretch()

        self._checkBox = QCheckBox()
        self._checkBox.stateChanged.connect(self._signal_checkBox_stateChanged)
        self._rootLayout.addWidget(self._checkBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_checkBox_stateChanged(self):
        self._push(self._checkBox.isChecked())
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        self._checkBox.setChecked(self._val.getVal())