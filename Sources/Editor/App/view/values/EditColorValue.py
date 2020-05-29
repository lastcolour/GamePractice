from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton, QColorDialog
from PyQt5.QtCore import Qt
from PyQt5.Qt import QPalette, QColor

class EditColorValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value
        r, g, b, a = self._val.getVal()

        self._rootLayout = QHBoxLayout()

        self._colorLineEdit = QLineEdit()
        self._colorLineEdit.setReadOnly(True)
        self._rootLayout.addWidget(self._colorLineEdit)

        self._colorSelectBt = QPushButton()
        self._colorSelectBt.setFlat(True)

        self._colorSelectBt.clicked.connect(self._signal_colorSelectBt_clicked)
        self._rootLayout.addWidget(self._colorSelectBt)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._updateWith(QColor(r, g, b, a))

    def _updateWith(self, col):
        palette = self._colorSelectBt.palette()
        palette.setColor(QPalette.Button, col)
        self._colorSelectBt.setAutoFillBackground(True)
        self._colorSelectBt.setPalette(palette)
        self._colorSelectBt.update()
        self._colorLineEdit.setText("{}".format(col.name()))

    def _signal_colorSelectBt_clicked(self):
        r, g, b, a = self._val.getVal()
        resColor = QColorDialog.getColor(QColor(r, g, b, a), self, "Select Color")
        if not resColor.isValid():
            return
        r = resColor.red()
        g = resColor.green()
        b = resColor.blue()
        a = resColor.alpha()
        self._val.setVal(r, g, b, a)
        self._updateWith(resColor)