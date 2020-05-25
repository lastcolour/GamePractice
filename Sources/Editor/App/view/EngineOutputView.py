from PyQt5.QtWidgets import QWidget
from PyQt5.QtCore import Qt

class EngineOutputView(QWidget):
    def __init__(self):
        super().__init__()
        palette = self.palette()
        palette.setColor(self.backgroundRole(), Qt.black)
        self.setPalette(palette)
        self.setAutoFillBackground(True)