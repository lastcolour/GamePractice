from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton
from PyQt5.QtCore import Qt

from dialog.SelectFromEntityChild import SelectFromEntityChild

def _getChildPath(childEntity):
    resPath = childEntity.getName()
    currEntity = childEntity.getParent()
    while currEntity is not None and currEntity.getParent() is not None:
        resPath = "{1}/{0}".format(resPath, currEntity.getName())
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
        self._pull()

    def _signal_selectBt_clicked(self):
        dialog = SelectFromEntityChild(self._val.getEntity())
        dialog.exec()
        res = dialog.getSelectedEntity()
        if res is None:
            return
        self._push(res)
        self._pull()

    def _push(self, entity):
        self._val.setEntityValue(entity)

    def _pull(self):
        entity = self._val.getEntityValue()
        if entity is None:
            self._lineEdit.setText("")
        else:
            self._lineEdit.setText(_getChildPath(entity))