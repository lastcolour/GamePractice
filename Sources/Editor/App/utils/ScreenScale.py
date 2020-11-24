import os
from PyQt5.QtWidgets import QApplication
from PyQt5 import QtCore

def SetupScreenScale():
    os.environ["QT_AUTO_SCREEN_SCALE_FACTOR"] = "1"
    QApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling)