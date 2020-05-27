from PyQt5.QtWidgets import QWidget, QSizePolicy
from PyQt5.QtCore import Qt, QSize, QPoint, QTimer
from PyQt5.QtGui import QPainter, QImage


from utils.EventManager import GetEventManager

class AspecRatio:
    Ratio_4x3 = 0
    Ratio_16x9 = 1
    Ratio_16x10 = 2

class EngineOutputFrameView(QWidget):
    def __init__(self):
        super().__init__()
        self._ratio = AspecRatio.Ratio_4x3
        self._image = QImage(QSize(400, 300), QImage.Format_RGBA8888)
        self._image.fill(Qt.black)
        self._draw = None

        self._tickTimer = QTimer()
        self._tickTimer.timeout.connect(self._onTick)
        self._tickTimer.start()

    def _getHeightFromWidth(self, width):
        if self._ratio == AspecRatio.Ratio_4x3:
            return width * 3 / 4
        elif self._ratio == AspecRatio.Ratio_16x10:
            return width * 10 / 16
        elif self._ratio == AspecRatio.Ratio_16x9:
            return width * 9 / 16
        else:
            raise RuntimeError("Invalid aspect ratio")

    def _onTick(self):
        self.update()
        if self._draw == 1024:
            raise RuntimeError("Error")

    def resizeEvent(self, event):
        size = event.size()
        height = self._getHeightFromWidth(size.width())
        scaleFactor = 1
        if height > size.height():
            scaleFactor = size.height() / height
        newWidth = max(size.width() * scaleFactor, 1)
        newHeight = max(height * scaleFactor, 1)
        self._image = self._image.scaled(newWidth, newHeight)

    def paintEvent(self, event):
        painter = QPainter()
        painter.begin(self)
        GetEventManager().drawNativeFrameTo(self._image.bits().__int__(), self._image.width(), self._image.height())
        painter.drawImage(QPoint(0, 0), self._image)
        painter.end()
        self._draw = 1