from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt

from view.values.EditWidgets import EditFloatSpinBox

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditVec2Value(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xSpinBox = EditFloatSpinBox()
        self._xSpinBox.valueChanged.connect(self._signal_xSpinBox_valueChanged)
        self._rootLayout.addWidget(self._xSpinBox)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._ySpinBox = EditFloatSpinBox()
        self._ySpinBox.valueChanged.connect(self._signal_ySpinBox_valueChanged)
        self._rootLayout.addWidget(self._ySpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xSpinBox_valueChanged(self, newXVal):
        x, y = self._val.getVal()
        self._push(newXVal, y)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _signal_ySpinBox_valueChanged(self, newYVal):
        x, y = self._val.getVal()
        self._push(x, newYVal)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _pull(self):
        x, y = self._val.getVal()
        self._xSpinBox.setValue(x)
        self._ySpinBox.setValue(y)

    def _push(self, x, y):
        self._val.setVal(x, y)