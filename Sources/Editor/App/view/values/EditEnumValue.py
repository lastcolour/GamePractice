from PyQt5.QtWidgets import QComboBox, QWidget, QHBoxLayout

class EditEnumValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()
        self._rootLayout.addStretch()

        self._comboBox = QComboBox()
        enumItems = [x for x in self._val.getTable()]
        enumItems.sort()
        for item in enumItems:
            self._comboBox.addItem(item)
        self._comboBox.currentTextChanged.connect(self._signal_comboBox_textChanged)
        self._rootLayout.addWidget(self._comboBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_comboBox_textChanged(self, text):
        self._push(text)
        self._pull()

    def _push(self, text):
        self._val.setVal(text)

    def _pull(self):
        idx = self._comboBox.findText(self._val.getVal())
        if idx == -1:
            raise RuntimeError("Can't find value '{0}' in combo box".format(self._val.getVal()))
        self._comboBox.setCurrentIndex(idx)