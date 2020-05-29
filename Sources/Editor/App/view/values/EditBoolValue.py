from PyQt5.QtWidgets import QCheckBox, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt

class EditBoolValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()
        self._rootLayout.addStretch()

        self._checkBox = QCheckBox()
        self._checkBox.stateChanged.connect(self._signal_checkBox_stateChanged)
        self._rootLayout.addWidget(self._checkBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_checkBox_stateChanged(self, state):
        if state == Qt.Checked:
            self._push(True)
        else:
            self._push(False)
        self._pull()

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        self._checkBox.setChecked(self._val.getVal())