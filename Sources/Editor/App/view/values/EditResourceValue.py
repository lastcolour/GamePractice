from PyQt6.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton
from PyQt6.QtCore import Qt

from dialog.UniversalSelectDialog import ExecResourceSelectDialog

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

from view.values.UIUitls import FreezeEditButtonSize

class EditResourceValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._lineEdit = QLineEdit()
        self._lineEdit.setAlignment(Qt.AlignmentFlag.AlignRight)
        self._lineEdit.setReadOnly(True)
        self._rootLayout.addWidget(self._lineEdit)

        self._clearBt = QPushButton("x")
        FreezeEditButtonSize(self._clearBt)
        self._clearBt.clicked.connect(self._signal_clearBt_clicked)
        self._rootLayout.addWidget(self._clearBt)

        self._selectBt = QPushButton("Select")
        self._selectBt.clicked.connect(self._signal_selectBt_clicked)
        self._rootLayout.addWidget(self._selectBt)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_clearBt_clicked(self):
        self._val._push(None)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _signal_selectBt_clicked(self):
        res = ExecResourceSelectDialog(self._val.getSpecType())
        if res is None:
            return
        self._push(res.getRelativePath())
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, resource):
        if resource != None:
            storageVal = self._val.getStorage()
            storageVal.setVal(resource)
        isSet = resource != None
        self._val.setIsSet(isSet)

    def _pull(self):
        if not self._val.isStorageSet():
            self._lineEdit.setText("")
        else:
            storageVal = self._val.getStorage().getVal()
            self._lineEdit.setText(storageVal)