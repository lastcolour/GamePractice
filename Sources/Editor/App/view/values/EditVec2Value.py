from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel

from .FloatLineEdit import FloatLineEdit

class EditVec2Value(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xLineEdit = FloatLineEdit()
        self._xLineEdit.textEdited.connect(self._signal_xEdit_textEdited)
        self._rootLayout.addWidget(self._xLineEdit)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._yLineEdit = FloatLineEdit()
        self._yLineEdit.textEdited.connect(self._signal_yEdit_textEdited)
        self._rootLayout.addWidget(self._yLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xEdit_textEdited(self, text):
        x, y = self._val.getVal()
        self._push(self._xLineEdit._getFloat(), y)
        self._pull()

    def _signal_yEdit_textEdited(self, text):
        x, y = self._val.getVal()
        self._push(x, self._yLineEdit._getFloat())
        self._pull()

    def _pull(self):
        x, y = self._val.getVal()
        self._xLineEdit._setFloat(x)
        self._yLineEdit._setFloat(y)

    def _push(self, x, y):
        self._val.setVal(x, y)