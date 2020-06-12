from PyQt5.QtWidgets import QLineEdit
from PyQt5.Qt import QDoubleValidator
from PyQt5.QtCore import Qt

class FloatLineEdit(QLineEdit):
    def __init__(self):
        super().__init__()
        self.setAlignment(Qt.AlignRight)
        self.setValidator(QDoubleValidator())

    def _getFloat(self):
        text = self.text()
        if text == "":
            return 0.0
        return float(text)

    def _setFloat(self, val):
        currText = self.text()
        newText = "{0:.3f}".format(val)
        if currText == newText:
            return
        cPos = self.cursorPosition()
        self.setText("{0:.3f}".format(val))
        self.setCursorPosition(cPos)