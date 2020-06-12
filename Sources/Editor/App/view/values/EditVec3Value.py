from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel

from .FloatLineEdit import FloatLineEdit

class EditVec3Value(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xLineEdit = FloatLineEdit()
        self._xLineEdit.textEdited.connect(self._signal_xEdit_textEdited)
        self._rootLayout.addWidget(self._xLineEdit)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._yLineEdit = FloatLineEdit()
        self._yLineEdit.textEdited.connect(self._signal_yEdit_textEdited)
        self._rootLayout.addWidget(self._yLineEdit)

        self._zLabel = QLabel("<b>Z:</>")
        self._rootLayout.addWidget(self._zLabel)

        self._zLineEdit = FloatLineEdit()
        self._zLineEdit.textEdited.connect(self._signal_zEdit_textEdited)
        self._rootLayout.addWidget(self._zLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xEdit_textEdited(self, text):
        x, y, z = self._val.getVal()
        self._push(self._xLineEdit._getFloat(), y, z)
        self._pull()

    def _signal_yEdit_textEdited(self, text):
        x, y, z = self._val.getVal()
        self._push(x, self._yLineEdit._getFloat(), z)
        self._pull()

    def _signal_zEdit_textEdited(self, text):
        x, y, z = self._val.getVal()
        self._push(x, y, self._zLineEdit._getFloat())
        self._pull()

    def _pull(self):
        x, y, z = self._val.getVal()
        self._xLineEdit._setFloat(x)
        self._yLineEdit._setFloat(y)
        self._zLineEdit._setFloat(z)

    def _push(self, x, y, z):
        self._val.setVal(x, y, z)