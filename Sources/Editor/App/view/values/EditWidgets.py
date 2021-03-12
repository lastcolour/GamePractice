from PyQt5.QtWidgets import QSpinBox, QDoubleSpinBox, QComboBox
from PyQt5.QtCore import Qt

class EditFloatSpinBox(QDoubleSpinBox):
    def __init__(self):
        super().__init__()
        self.setMaximum(1000000.0)
        self.setMinimum(-1000000.0)
        self.setSingleStep(0.1)
        self.setAlignment(Qt.AlignRight)
        self.setFocusPolicy(Qt.StrongFocus)

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
        self.setAlignment(Qt.AlignRight)
        self.setFocusPolicy(Qt.StrongFocus)

    def wheelEvent(self, event):
        if self.hasFocus():
            super().wheelEvent(event)
        else:
            event.ignore()

class EditComboBox(QComboBox):
    def __init__(self):
        super().__init__()
        self.setFocusPolicy(Qt.StrongFocus)

    def wheelEvent(self, event):
        if self.hasFocus():
            super().wheelEvent(event)
        else:
            event.ignore()