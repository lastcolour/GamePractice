from PyQt5.QtWidgets import QWidget, QSizePolicy
from PyQt5.QtCore import Qt, QSize, QPoint, QTimer
from PyQt5.QtGui import QPainter, QImage, QResizeEvent

from utils.EventManager import GetEventManager
from utils.EngineViewManager import GetEngineViewManager

class AspecRatio:
    Ratio_4x3 = 0
    Ratio_16x9 = 1
    Ratio_16x10 = 2
    Ratio_5x3 = 3
    Ratio_18x9 = 4

_MIN_WIDTH = 150

class EngineOutputFrameView(QWidget):

    def __init__(self):
        super().__init__()
        self._ratio = AspecRatio.Ratio_16x9
        self._isHorizontal = False

        self._tickTimer = QTimer()
        self._tickTimer.timeout.connect(self._onTick)
        self._tickTimer.start()

        self._image = QImage(QSize(_MIN_WIDTH, self._getHeightFromWidth(_MIN_WIDTH)), QImage.Format_RGBA8888)
        self._image.fill(Qt.black)
        self.setMinimumSize(QSize(_MIN_WIDTH, _MIN_WIDTH))

    def _getHeightFromWidth(self, width):
        x = None
        y = None
        if self._ratio == AspecRatio.Ratio_4x3:
            x = 4
            y = 3
        elif self._ratio == AspecRatio.Ratio_16x9:
            x = 16
            y = 9
        elif self._ratio == AspecRatio.Ratio_16x10:
            x = 16
            y = 10
        elif self._ratio == AspecRatio.Ratio_5x3:
            x = 5
            y = 3
        elif self._ratio == AspecRatio.Ratio_18x9:
            x = 18
            y = 9
        else:
            raise RuntimeError("Unknown aspect ratio: '{0}'".format(self._ratio))
        if self._isHorizontal:
            return int(width * y / x)
        else:
            return int(width * x / y)

    def _onTick(self):
        self.update()

    def resizeEvent(self, event):
        size = event.size()
        height = self._getHeightFromWidth(size.width())
        scaleFactor = 1
        if height > size.height():
            scaleFactor = size.height() / height
        newWidth = int(max(size.width() * scaleFactor, 1))
        newHeight = int(max(height * scaleFactor, 1))
        self._image = self._image.scaled(newWidth, newHeight)
        GetEngineViewManager().onOutputFrameSizeChanged(newWidth, newHeight)

    def _getDrawPoint(self):
        sz = self.size()
        x = (sz.width() - self._image.width()) / 2
        y = (sz.height() - self._image.height()) / 2
        return QPoint(int(x), int(y))

    def paintEvent(self, event):
        GetEngineViewManager().drawNativeFrameTo(self._image.bits().__int__(), self._image.width(), self._image.height())
        self._image = self._image.mirrored(vertical=True)

        painter = QPainter()
        painter.begin(self)
        painter.drawImage(self._getDrawPoint(), self._image)
        painter.end()

    def setAspectRatio(self, ratio):
        self._ratio = ratio
        newHeight = self._getHeightFromWidth(self.size().width())
        self._image = self._image.scaled(self._image.width(), newHeight)
        self._image.fill(Qt.black)
        self.resizeEvent(QResizeEvent(self.size(), QSize()))

    def setOrientation(self, isHorizontal):
        self._isHorizontal = isHorizontal
        self.setAspectRatio(self._ratio)