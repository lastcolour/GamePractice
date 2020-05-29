from PyQt5.QtWidgets import QWidget, QGridLayout, QLabel

from .EditVec3Value import EditVec3Value

class EditTransformValue(QWidget):
    def __init__(self, editEntity):
        super().__init__()

        self._editEntity = editEntity

        self._rootLayout = QGridLayout()

        self._posLabel = QLabel("Position")
        self._rootLayout.addWidget(self._posLabel, 0, 0)

        self._posValue = EditVec3Value(None)
        self._rootLayout.addWidget(self._posValue, 0, 1)

        self._scaleLabel = QLabel("Scale")
        self._rootLayout.addWidget(self._scaleLabel, 1, 0)

        self._scaleValue = EditVec3Value(None)
        self._rootLayout.addWidget(self._scaleValue, 1, 1)

        self._rotLabel = QLabel("Rotation")
        self._rootLayout.addWidget(self._rotLabel, 2, 0)

        self._rotValue = EditVec3Value(None)
        self._rootLayout.addWidget(self._rotValue, 2, 1)

        self.setLayout(self._rootLayout)