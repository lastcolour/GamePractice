from PyQt5.QtWidgets import QWidget, QVBoxLayout

from .EngineOutputFrameView import EngineOutputFrameView

class EngineOutputView(QWidget):
    def __init__(self):
        super().__init__()

        self._rootLayout = QVBoxLayout()

        self._engineOutFrame = EngineOutputFrameView()
        self._rootLayout.addWidget(self._engineOutFrame)

        self.setLayout(self._rootLayout)