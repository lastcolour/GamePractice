from PyQt5.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton
from PyQt5.QtCore import Qt

from dialog.SelectFromEntityChild import SelectFromEntityChild

class EditEntityValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._lineEdit = QLineEdit()
        self._lineEdit.setAlignment(Qt.AlignRight)
        self._lineEdit.setReadOnly(True)
        self._rootLayout.addWidget(self._lineEdit)

        self._selectBt = QPushButton("Select")
        self._selectBt.clicked.connect(self._signal_selectBt_clicked)
        self._rootLayout.addWidget(self._selectBt)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

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
            self._lineEdit.setText(entity.getName())