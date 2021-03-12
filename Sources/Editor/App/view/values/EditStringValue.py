from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout
from PyQt5.QtCore import Qt

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditStringValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._strLineEdit = QLineEdit()
        self._strLineEdit.textEdited.connect(self._signal_lineEdit_textEdited)
        self._strLineEdit.setAlignment(Qt.AlignRight)
        self._rootLayout.addWidget(self._strLineEdit)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_lineEdit_textEdited(self, text):
        self._push(text)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, data):
        self._val.setVal(data)

    def _pull(self):
        self._strLineEdit.setText(self._val.getVal())