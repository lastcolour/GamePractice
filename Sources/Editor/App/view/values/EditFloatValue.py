from PyQt5.QtWidgets import QWidget, QHBoxLayout
from PyQt5.QtCore import Qt

from .FloatLineEdit import FloatLineEdit

class EditFloatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._floatLineEdit = FloatLineEdit()
        self._floatLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._rootLayout.addWidget(self._floatLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_lineEdit_textEdited(self, text):
        self._push(self._floatLineEdit._getFloat())
        self._pull()

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        data = self._val.getVal()
        self._floatLineEdit._setFloat(data)