from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt
from PyQt5.Qt import QDoubleValidator

class EditFloatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._floatLineEdit = QLineEdit()
        self._floatLineEdit.setAlignment(Qt.AlignRight)
        self._floatLineEdit.setText("{0}".format(self._val.getVal()))
        self._floatLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._floatLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)