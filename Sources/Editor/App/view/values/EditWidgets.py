from PyQt6.QtWidgets import QSpinBox, QDoubleSpinBox, QComboBox
from PyQt6.QtCore import Qt

import math

class EditFloatSpinBox(QDoubleSpinBox):
    def __init__(self):
        super().__init__()
        self.setMaximum(1000000.0)
        self.setMinimum(-1000000.0)
        self.setDecimals(5)
        self.setSingleStep(0.1)
        self.setAlignment(Qt.AlignmentFlag.AlignRight)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

    def valueFromText(self, s):
        return float(s)

    def textFromValue(self, val):
        if self.hasFocus():
            if math.isclose(float(self.text()), val):
                return self.text()
        return "{0}".format(val)

    def wheelEvent(self, event):
        if self.hasFocus():
            super().wheelEvent(event)
        else:
            event.ignore()

class EditIntSpinBox(QSpinBox):
    def __init__(self):
        super().__init__()
        self.setMaximum(1000000)
        self.setMinimum(-1000000)
        self.setAlignment(Qt.AlignmentFlag.AlignRight)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

    def wheelEvent(self, event):
        if self.hasFocus():
            super().wheelEvent(event)
        else:
            event.ignore()

class EditComboBox(QComboBox):
    def __init__(self):
        super().__init__()
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

    def wheelEvent(self, event):
        if self.hasFocus():
            super().wheelEvent(event)
        else:
            event.ignore()