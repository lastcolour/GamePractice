from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt
from PyQt5.Qt import QIntValidator

class EditIntValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._intLineEdit = QLineEdit()
        self._intLineEdit.setAlignment(Qt.AlignRight)
        self._intLineEdit.setText("{0}".format(self._val.getVal()))
        self._intLineEdit.setValidator(QIntValidator())
        self._rootLayout.addWidget(self._intLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)