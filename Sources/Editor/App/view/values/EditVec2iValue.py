from PyQt5.QtWidgets import QSpinBox, QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt

class EditVec2iValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xSpinBox = QSpinBox()
        self._xSpinBox.setMinimum(-1000000)
        self._xSpinBox.setMaximum(1000000)
        self._xSpinBox.setAlignment(Qt.AlignRight)
        self._xSpinBox.valueChanged.connect(self._signal_xSpinBox_valueChanged)
        self._rootLayout.addWidget(self._xSpinBox)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._ySpinBox = QSpinBox()
        self._ySpinBox.setMinimum(-1000000)
        self._ySpinBox.setMaximum(1000000)
        self._ySpinBox.setAlignment(Qt.AlignRight)
        self._ySpinBox.valueChanged.connect(self._signal_ySpinBox_valueChanged)
        self._rootLayout.addWidget(self._ySpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xSpinBox_valueChanged(self, newXValue):
        x, y = self._val.getVal()
        self._push(newXValue, y)
        self._pull()

    def _signal_ySpinBox_valueChanged(self, newYValue):
        x, y = self._val.getVal()
        self._push(x, newYValue)
        self._pull()

    def _pull(self):
        x, y = self._val.getVal()
        self._xSpinBox.setValue(x)
        self._ySpinBox.setValue(y)

    def _push(self, x, y):
        self._val.setVal(x, y)