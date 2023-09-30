from PyQt6.QtWidgets import QLineEdit, QWidget, QHBoxLayout, QPushButton, QColorDialog, QFrame
from PyQt6.QtCore import Qt 
from PyQt6.QtGui import QColor, QPalette

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

class EditColorValue(QWidget):

    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._colorLineEdit = QLineEdit()
        self._colorLineEdit.setAlignment(Qt.AlignmentFlag.AlignRight)
        self._colorLineEdit.setReadOnly(True)
        self._rootLayout.addWidget(self._colorLineEdit)

        self._frame = QFrame()
        self._frame.setFrameStyle(QFrame.Shape.StyledPanel)
        self._frame.setFrameShadow(QFrame.Shadow.Plain)
        self._frameLayout = QHBoxLayout()

        self._colorSelectBt = QPushButton()
        self._colorSelectBt.setFlat(True)
        self._colorSelectBt.clicked.connect(self._signal_colorSelectBt_clicked)

        self._frameLayout.setContentsMargins(0, 0, 0, 0)
        self._frameLayout.setSpacing(0)
        self._frameLayout.addWidget(self._colorSelectBt)
        self._frame.setMaximumHeight(self._colorSelectBt.minimumSizeHint().height() - 2)
        self._frame.setLayout(self._frameLayout)

        self._rootLayout.addWidget(self._frame)
        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _updateWith(self, col):
        palette = self._colorSelectBt.palette()
        palette.setColor(QPalette.ColorRole.Button, col)
        self._colorSelectBt.setAutoFillBackground(True)
        self._colorSelectBt.setPalette(palette)
        self._colorSelectBt.update()
        self._colorLineEdit.setText("<{0}, {1}, {2}, {3}>".format(
            col.red(), col.green(), col.blue(), col.alpha()))

    def _signal_colorSelectBt_clicked(self):
        r, g, b, a = self._val.getVal()
        resColor = QColorDialog.getColor(QColor(r, g, b, a), self, "Select Color",
            QColorDialog.ColorDialogOption.ShowAlphaChannel | QColorDialog.ColorDialogOption.DontUseNativeDialog)
        if not resColor.isValid():
            return
        self._push(resColor)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _push(self, col):
        self._val.setVal(col.red(), col.green(), col.blue(), col.alpha())

    def _pull(self):
        r, g, b, a = self._val.getVal()
        self._updateWith(QColor(r, g, b, a))