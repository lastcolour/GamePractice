from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt
from PyQt5.Qt import QDoubleValidator

class EditVec3Value(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value
        xVal, yVal, zVal = self._val.getVal()

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</>")
        self._rootLayout.addWidget(self._xLabel)

        self._xLineEdit = QLineEdit()
        self._xLineEdit.setAlignment(Qt.AlignRight)
        self._xLineEdit.setText("{0}".format(xVal))
        self._xLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._xLineEdit)

        self._yLabel = QLabel("<b>Y:</>")
        self._rootLayout.addWidget(self._yLabel)

        self._yLineEdit = QLineEdit()
        self._yLineEdit.setAlignment(Qt.AlignRight)
        self._yLineEdit.setText("{0}".format(yVal))
        self._yLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._yLineEdit)

        self._zLabel = QLabel("<b>Z:</>")
        self._rootLayout.addWidget(self._zLabel)

        self._zLineEdit = QLineEdit()
        self._zLineEdit.setAlignment(Qt.AlignRight)
        self._zLineEdit.setText("{0}".format(zVal))
        self._zLineEdit.setValidator(QDoubleValidator())
        self._rootLayout.addWidget(self._zLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)