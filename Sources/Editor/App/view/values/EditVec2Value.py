from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt
from PyQt5.Qt import QDoubleValidator

from .Utils import TextToFloat, SetFloatToLineEdit

class EditVec2Value(QWidget):
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

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xEdit_textEdited(self, text):
        x, y = self._val.getVal()
        self._push(TextToFloat(text), y)
        self._pull()

    def _signal_yEdit_textEdited(self, text):
        x, y = self._val.getVal()
        self._push(x, TextToFloat(text))
        self._pull()

    def _pull(self):
        x, y = self._val.getVal()
        SetFloatToLineEdit(self._xLineEdit, x)
        SetFloatToLineEdit(self._yLineEdit, y)

    def _push(self, x, y):
        self._val.setVal(x, y)