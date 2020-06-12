from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel

from .FloatLineEdit import FloatLineEdit

def _convertToEualerAngles(x, y, z, w):
    return x, y, z

def _convertFromEualerAngles(x, y, z):
    return x, y, z, 0

class EditQuatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xLineEdit = FloatLineEdit()
        self._xLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._rootLayout.addWidget(self._xLineEdit)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._yLineEdit = FloatLineEdit()
        self._yLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._rootLayout.addWidget(self._yLineEdit)

        self._zLabel = QLabel("<b>Z:</>")
        self._rootLayout.addWidget(self._zLabel)

        self._zLineEdit = FloatLineEdit()
        self._zLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._rootLayout.addWidget(self._zLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_lineEdit_textEdited(self, text):
        x = self._xLineEdit._getFloat()
        y = self._zLineEdit._getFloat()
        z = self._yLineEdit._getFloat()
        xVal, yVal, zVal, wVal = _convertFromEualerAngles(x, y, z)

        self._push(xVal, yVal, zVal, wVal)
        self._pull()

    def _pull(self):
        x, y, z, w = self._val.getVal()
        xAngle, yAngle, zAngle = _convertToEualerAngles(x, y, z, w)
        self._xLineEdit._setFloat(xAngle)
        self._yLineEdit._setFloat(yAngle)
        self._zLineEdit._setFloat(zAngle)

    def _push(self, x, y, z, w):
        self._val.setVal(x, y, z, w)