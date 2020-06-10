import PyQt5.QtWidgets import QLineEdit
from PyQt5.Qt import QDoubleValidator
from PyQt5.QtCore import Qt

class FloatLineEdit(QLineEdit):
    def __init__(self):
        super().__init__()
        self.setAlignment(Qt.AlignRight)
        self.setValidator(QDoubleValidator())

    def _getFloat(self):
        pass

    def _setFloat(self):
        pass