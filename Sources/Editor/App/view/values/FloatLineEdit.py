from PyQt5.QtWidgets import QLineEdit
from PyQt5.Qt import QDoubleValidator
from PyQt5.QtCore import Qt

from utils.Log import Log

class FloatLineEdit(QLineEdit):
    def __init__(self):
        super().__init__()
        self.setAlignment(Qt.AlignRight)
        self.setValidator(QDoubleValidator())

    def _getFloat(self):
        text = self.text()
        if text == "":
            return 0.0
        try:
            floatVal = float(text)
        except Exception as e:
            print("[FloatLineEdit:_getFloat] Can't get float from: '{0}' (Error: {1})".format(
                text, e))
            floatVal = 0.0
        return floatVal

    def _setFloat(self, val):
        try:
            floatVal = float(val)
        except Exception as e:
            print("[FloatLineEdit:_setFloat] Can't set float to: '{0}' (Error: {1})".format(
                val, e))
            floatVal = 0.0
        currText = self.text()
        newText = "{0:.3f}".format(floatVal)
        if currText == newText:
            return
        cPos = self.cursorPosition()
        self.setText("{0:.3f}".format(floatVal))
        self.setCursorPosition(cPos)