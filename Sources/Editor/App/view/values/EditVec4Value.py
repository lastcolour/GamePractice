from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel, QDoubleSpinBox
from PyQt5.QtCore import Qt

class EditVec4Value(QWidget):
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

        self._wLabel = QLabel("<b>W:</>")
        self._rootLayout.addWidget(self._wLabel)

        self._wSpinBox = QDoubleSpinBox()
        self._wSpinBox.setMaximum(1000000.0)
        self._wSpinBox.setMinimum(-1000000.0)
        self._wSpinBox.setSingleStep(0.1)
        self._wSpinBox.setAlignment(Qt.AlignRight)
        self._wSpinBox.valueChanged.connect(self._signal_wSpinBox_valueChanged)
        self._rootLayout.addWidget(self._wSpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xSpinBox_valueChanged(self, newXVal):
        x, y, z, w = self._val.getVal()
        self._push(newXVal, y, z, w)
        self._pull()

    def _signal_ySpinBox_valueChanged(self, newYVal):
        x, y, z, w = self._val.getVal()
        self._push(x, newYVal, z, w)
        self._pull()

    def _signal_zSpinBox_valueChanged(self, newZVal):
        x, y, z, w = self._val.getVal()
        self._push(x, y, newZVal, w)
        self._pull()

    def _signal_wSpinBox_valueChanged(self, newWVal):
        x, y, z, w = self._val.getVal()
        self._push(x, y, z, newWVal)
        self._pull()

    def _pull(self):
        x, y, z, w = self._val.getVal()
        self._xSpinBox.setValue(x)
        self._ySpinBox.setValue(y)
        self._zSpinBox.setValue(z)
        self._wSpinBox.setValue(w)

    def _push(self, x, y, z, w):
        self._val.setVal(x, y, z, w)