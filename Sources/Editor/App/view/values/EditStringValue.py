from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt

class EditStringValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._strLineEdit = QLineEdit()
        self._strLineEdit.setAlignment(Qt.AlignRight)
        self._strLineEdit.setText(self._val.getVal())
        self._rootLayout.addWidget(self._strLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)