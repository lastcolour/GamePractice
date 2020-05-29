from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt
from PyQt5.Qt import QIntValidator

from .Utils import TextToInt, SetIntToLineEdit

class EditIntValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._intLineEdit = QLineEdit()
        self._intLineEdit.setAlignment(Qt.AlignRight)
        self._intLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._intLineEdit.setValidator(QIntValidator())
        self._rootLayout.addWidget(self._intLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_lineEdit_textEdited(self, text):
        self._push(TextToInt(text))
        self._pull()

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        data = self._val.getVal()
        SetIntToLineEdit(self._intLineEdit, data)