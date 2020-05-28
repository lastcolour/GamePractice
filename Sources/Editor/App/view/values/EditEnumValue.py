from PyQt5.QtWidgets import QComboBox, QWidget, QHBoxLayout

class EditEnumValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()
        self._rootLayout.addStretch()

        self._comboBox = QComboBox()
        for item in self._val.getTable():
            self._comboBox.addItem(item)
        idx = self._comboBox.findData(self._val.getVal())
        if idx == -1:
            raise RuntimeError("Can't find value '{0}' in table".format(self._val.getVal()))
        self._comboBox.setCurrentIndex(idx)
        self._rootLayout.addWidget(self._checkBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)