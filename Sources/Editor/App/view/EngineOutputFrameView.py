from PyQt5.QtWidgets import QWidget, QSizePolicy
from PyQt5.QtCore import Qt, QSize, QPoint, QTimer
from PyQt5.QtGui import QPainter, QImage, QResizeEvent

from utils.Managers import GetEventManager
from utils.Managers import GetEngineViewManager
from utils.EngineViewManager import MouseEventType

class AspecRatio:
    Ratio_4x3 = 0
    Ratio_16x9 = 1
    Ratio_16x10 = 2
    Ratio_5x3 = 3
    Ratio_18x9 = 4
    Ratio_21x9 = 5
    Ratio_None = 6

_MIN_WIDTH = 150

class EngineOutputFrameView(QWidget):

    def __init__(self):
        super().__init__()
        self._ratio = AspecRatio.Ratio_None
        self._isHorizontal = False
        self._isMousePressed = False
        self._lastMousePt = (0, 0)

        self._tickTimer = QTimer()
        self._tickTimer.timeout.connect(self._onTick)
        self._tickTimer.start()

        self._image = QImage(QSize(_MIN_WIDTH, self._getHeightFromWidth(_MIN_WIDTH)), QImage.Format_RGBA8888)
        self._image.fill(Qt.black)
        self.setMinimumSize(QSize(_MIN_WIDTH, _MIN_WIDTH))
        self.setMouseTracking(True)

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
        elif self._ratio == AspecRatio.Ratio_21x9:
            x = 21
            y = 9
        elif self._ratio == AspecRatio.Ratio_None:
            return self.size().height()
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
        newWidth -= newWidth % 2
        newHeight = int(max(height * scaleFactor, 1))
        newHeight -= newHeight % 2
        self._image = self._image.scaled(newWidth, newHeight)
        GetEngineViewManager().onOutputFrameSizeChanged(newWidth, newHeight)

    def _getDrawPoint(self):
        sz = self.size()
        x = (sz.width() - self._image.width()) / 2
        y = (sz.height() - self._image.height()) / 2
        return QPoint(int(x), int(y))

    def paintEvent(self, event):
        w, h = self._image.width(), self._image.height()
        GetEngineViewManager().drawNativeFrameTo(self._image.bits().__int__(), w, h)

        painter = QPainter()
        painter.begin(self)
        painter.translate(w / 2, h / 2)
        painter.rotate(180)
        painter.scale(-1.0, 1.0) 
        painter.drawImage(-w / 2, -h / 2, self._image)
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

    def _getPosInside(self, event):
        pt = event.pos()
        x, y = pt.x(), pt.y()
        y = self.size().height() - y
        drawPt = self._getDrawPoint()
        minX = drawPt.x()
        minY = drawPt.y()
        maxX = minX + self._image.width()
        maxY = minY + self._image.height()
        if x < minX or x >= maxX:
            return None, None
        if y < minY or y >= maxY:
            return None, None
        x = x - minX
        y = y - minY
        return x, y

    def mousePressEvent(self, event):
        x, y = self._getPosInside(event)
        if x is None or y is None:
            return
        if event.button() != Qt.LeftButton:
            return
        self._lastMousePt = (x, y)
        self._isMousePressed = True
        GetEngineViewManager().onMouseInputEvent(MouseEventType.Press, x, y)

    def mouseReleaseEvent(self, event):
        x, y = self._getPosInside(event)
        if x is None or y is None:
            return
        if event.button() != Qt.LeftButton:
            return
        if self._isMousePressed == False:
            return
        self._isMousePressed = False
        GetEngineViewManager().onMouseInputEvent(MouseEventType.Release, x, y)
        self._lastMousePt = (0, 0)

    def mouseMoveEvent(self, event):
        if not self._isMousePressed:
            return
        x, y = self._getPosInside(event)
        if x is None or y is None:
            GetEngineViewManager().onMouseInputEvent(MouseEventType.Release,
                self._lastMousePt[0], self._lastMousePt[1])
            self._lastMousePt = (0, 0)
            self._isMousePressed = False
        else:
            self._lastMousePt = (x, y)
            GetEngineViewManager().onMouseInputEvent(MouseEventType.Move, x, y)