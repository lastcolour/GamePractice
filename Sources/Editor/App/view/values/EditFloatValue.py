from PyQt5.QtWidgets import QWidget, QHBoxLayout, QDoubleSpinBox
from PyQt5.QtCore import Qt

class EditFloatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._floatSpinBox = QDoubleSpinBox()
        self._floatSpinBox.setSingleStep(0.1)
        self._floatSpinBox.setMinimum(-1000000.0)
        self._floatSpinBox.setMaximum(1000000.0)
        self._floatSpinBox.setAlignment(Qt.AlignRight)
        self._floatSpinBox.valueChanged.connect(self._signal_floatSpinBox_valueChanged)
        self._rootLayout.addWidget(self._floatSpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_floatSpinBox_valueChanged(self, newValue):
        self._push(newValue)
        self._pull()

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        data = self._val.getVal()
        self._floatSpinBox.setValue(data)