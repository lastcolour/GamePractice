from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel, QDoubleSpinBox
from PyQt5.QtCore import Qt

class EditVec3Value(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xSpinBox = QDoubleSpinBox()
        self._xSpinBox.setMaximum(1000000.0)
        self._xSpinBox.setMinimum(-1000000.0)
        self._xSpinBox.setSingleStep(0.1)
        self._xSpinBox.setAlignment(Qt.AlignRight)
        self._xSpinBox.valueChanged.connect(self._signal_xSpinBox_valueChanged)
        self._rootLayout.addWidget(self._xSpinBox)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._ySpinBox = QDoubleSpinBox()
        self._ySpinBox.setMaximum(1000000.0)
        self._ySpinBox.setMinimum(-1000000.0)
        self._ySpinBox.setSingleStep(0.1)
        self._ySpinBox.setAlignment(Qt.AlignRight)
        self._ySpinBox.valueChanged.connect(self._signal_ySpinBox_valueChanged)
        self._rootLayout.addWidget(self._ySpinBox)

        self._zLabel = QLabel("<b>Z:</>")
        self._rootLayout.addWidget(self._zLabel)

        self._zSpinBox = QDoubleSpinBox()
        self._zSpinBox.setMaximum(1000000.0)
        self._zSpinBox.setMinimum(-1000000.0)
        self._zSpinBox.setSingleStep(0.1)
        self._zSpinBox.setAlignment(Qt.AlignRight)
        self._zSpinBox.valueChanged.connect(self._signal_zSpinBox_valueChanged)
        self._rootLayout.addWidget(self._zSpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xSpinBox_valueChanged(self, newXVal):
        x, y, z = self._val.getVal()
        self._push(newXVal, y, z)
        self._pull()

    def _signal_ySpinBox_valueChanged(self, newYVal):
        x, y, z = self._val.getVal()
        self._push(x, newYVal, z)
        self._pull()

    def _signal_zSpinBox_valueChanged(self, newZVal):
        x, y, z = self._val.getVal()
        self._push(x, y, newZVal)
        self._pull()

    def _pull(self):
        x, y, z = self._val.getVal()
        self._xSpinBox.setValue(x)
        self._ySpinBox.setValue(y)
        self._zSpinBox.setValue(z)

    def _push(self, x, y, z):
        self._val.setVal(x, y, z)