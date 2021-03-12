from PyQt5.QtWidgets import QWidget, QHBoxLayout
from PyQt5.QtCore import Qt

from view.values.EditWidgets import EditIntSpinBox

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditIntValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._intSpinBox = EditIntSpinBox()
        self._intSpinBox.valueChanged.connect(self._signal_spinBox_valueCanged)
        self._rootLayout.addWidget(self._intSpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_spinBox_valueCanged(self, val):
        self._push(val)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        data = self._val.getVal()
        self._intSpinBox.setValue(data)