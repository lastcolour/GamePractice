from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt
from PyQt5.Qt import QDoubleValidator

from .Utils import TextToFloat, SetFloatToLineEdit

class EditFloatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._floatLineEdit = QLineEdit()
        self._floatLineEdit.setAlignment(Qt.AlignRight)
        self._floatLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._floatLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._floatLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_lineEdit_textEdited(self, text):
        self._push(TextToFloat(text))
        self._pull()

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        data = self._val.getVal()
        SetFloatToLineEdit(self._floatLineEdit, data)