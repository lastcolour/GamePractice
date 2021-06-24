from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton
from PyQt5.QtCore import Qt

from dialog.SelectFromEntityChild import SelectFromEntityChild

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

from view.values.UIUitls import FreezeEditButtonSize

def _getChildPath(thisEntity, childEntity):
    if childEntity.isInternal():
        resPath = childEntity.getName()
    else:
        resPath = "{0} {1}".format(childEntity.getName(), childEntity.getNameSuffix())
    currEntity = childEntity.getParent()
    while currEntity != thisEntity:
        if currEntity.isInternal():
            resPath = "{0}/{1}".format(currEntity.getName(), resPath)
        else:
            resPath = "{0} {1}/{2}".format(currEntity.getName(), currEntity.getNameSuffix(), resPath)
        currEntity = currEntity.getParent()
    return resPath

class EditEntityValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._lineEdit = QLineEdit()
        self._lineEdit.setAlignment(Qt.AlignRight)
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
        self._push(None)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _signal_selectBt_clicked(self):
        dialog = SelectFromEntityChild(self._val.getEntity())
        dialog.exec()
        res = dialog.getSelectedEntity()
        if res is None:
            return
        self._push(res)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, entity):
        self._val.setEntityValue(entity)

    def _pull(self):
        entity = self._val.getEntityValue()
        resText = "none"
        if entity is not None:
            if entity == self._val.getEntity():
                resText = "\"this\""
            else:
                resText = "\"this/{0}\"".format(_getChildPath(self._val.getEntity(), entity))
        self._lineEdit.setText(resText)