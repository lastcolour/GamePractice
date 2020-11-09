from PyQt5.QtWidgets import QFrame, QWidget, QVBoxLayout, QDockWidget
from PyQt5 import QtCore

from utils.Managers import GetMainViewManager

class _MainFrameWidget(QWidget):
    def __init__(self, widget):
        super().__init__()

        self._mainWidget = widget

        self._frameLayout = QVBoxLayout()
        self._frameLayout.setContentsMargins(0, 0, 0, 0)
        self._frameLayout.setSpacing(0)
        self._frameLayout.addWidget(self._mainWidget)

        self._frame = QFrame()
        self._frame.setFrameStyle(QFrame.StyledPanel)
        self._frame.setFrameShadow(QFrame.Plain)
        self._frame.setLayout(self._frameLayout)

        self._rootLayout = QVBoxLayout()
        self._rootLayout.setSpacing(0)
        self._rootLayout.setContentsMargins(0, 0, 0, 0)
        self._rootLayout.addWidget(self._frame)

        self.setLayout(self._rootLayout)

class _MainDock(QDockWidget):
    def __init__(self):
        super().__init__()
        self.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)

    def closeEvent(self, event):
        super().closeEvent(event)
        GetMainViewManager().onMainViewClosed(self.widget()._mainWidget)

def WrapMainDockWidget(widget, title):
    mainFrame = _MainFrameWidget(widget)
    mainDock = _MainDock()
    mainDock.setWindowTitle(title)
    mainDock.setWidget(mainFrame)
    return mainDock