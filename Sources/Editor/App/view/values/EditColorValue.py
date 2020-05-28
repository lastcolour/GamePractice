from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton
from PyQt5.QtCore import Qt
from PyQt5.Qt import QPalette, QColor

class EditColorValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value
        r, g, b, a = self._val.getVal()

        self._rootLayout = QHBoxLayout()

        self._colorLineEdit = QLineEdit()
        self._rootLayout.addWidget(self._colorLineEdit)

        self._colorSelectBt = QPushButton()
        palette = self._colorSelectBt.palette()
        palette.setColor(QPalette.Background, QColor(r, g, b, a))
        self._colorSelectBt.setAutoFillBackground(True)
        self._colorSelectBt.setPalette(palette)
        self._rootLayout.addWidget(self._colorSelectBt)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)