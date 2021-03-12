from PyQt5.QtWidgets import QWidget, QHBoxLayout
from PyQt5.QtCore import Qt

from view.values.EditWidgets import EditFloatSpinBox

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditFloatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._floatSpinBox = EditFloatSpinBox()
        self._floatSpinBox.valueChanged.connect(self._signal_floatSpinBox_valueChanged)
        self._rootLayout.addWidget(self._floatSpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_floatSpinBox_valueChanged(self, newValue):
        self._push(newValue)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        data = self._val.getVal()
        self._floatSpinBox.setValue(data)