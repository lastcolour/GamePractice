from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt
from PyQt5.Qt import QDoubleValidator

from .Utils import TextToFloat, SetFloatToLineEdit

class EditQuatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xLineEdit = QLineEdit()
        self._xLineEdit.textEdited.connect(self._signal_xEdit_textEdited)
        self._xLineEdit.setAlignment(Qt.AlignRight)
        self._xLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._xLineEdit)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._yLineEdit = QLineEdit()
        self._yLineEdit.textEdited.connect(self._signal_yEdit_textEdited)
        self._yLineEdit.setAlignment(Qt.AlignRight)
        self._yLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._yLineEdit)

        self._zLabel = QLabel("<b>Z:</>")
        self._rootLayout.addWidget(self._zLabel)

        self._zLineEdit = QLineEdit()
        self._zLineEdit.textEdited.connect(self._signal_zEdit_textEdited)
        self._zLineEdit.setAlignment(Qt.AlignRight)
        self._zLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._zLineEdit)

        self._wLabel = QLabel("<b>W:</>")
        self._rootLayout.addWidget(self._wLabel)

        self._wLineEdit = QLineEdit()
        self._wLineEdit.textEdited.connect(self._signal_wEdit_textEdited)
        self._wLineEdit.setAlignment(Qt.AlignRight)
        self._wLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._wLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xEdit_textEdited(self, text):
        x, y, z, w = self._val.getVal()
        self._push(TextToFloat(text), y, z, w)
        self._pull()

    def _signal_yEdit_textEdited(self, text):
        x, y, z, w = self._val.getVal()
        self._push(x, TextToFloat(text), z, w)
        self._pull()

    def _signal_zEdit_textEdited(self, text):
        x, y, z, w = self._val.getVal()
        self._push(x, y, TextToFloat(text), w)
        self._pull()

    def _signal_wEdit_textEdited(self, text):
        x, y, z, w = self._val.getVal()
        self._push(x, y, z, TextToFloat(text))
        self._pull()

    def _pull(self):
        x, y, z, w = self._val.getVal()
        SetFloatToLineEdit(self._xLineEdit, x)
        SetFloatToLineEdit(self._yLineEdit, y)
        SetFloatToLineEdit(self._zLineEdit, z)
        SetFloatToLineEdit(self._wLineEdit, w)

    def _push(self, x, y, z, w):
        self._val.setVal(x, y, z, w)