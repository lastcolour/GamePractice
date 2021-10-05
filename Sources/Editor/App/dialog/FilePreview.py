from PyQt5 import QtCore
from PyQt5.QtGui import QPalette, QPixmap
from PyQt5.QtWidgets import QSizePolicy, QWidget, QVBoxLayout, QLabel, QScrollArea

from model.AssetsModel import FileNodeType, FileNode
from utils.Log import Log

import os

def _convertSize(size):
    if size < 1024:
        return "{0} B".format(size)
    elif size < 1024 ** 2:
        return "{0:.1f} KB".format(size / 1024)
    else:
        return "{0:.1f} MB".format(size / (1024 ** 2))

def _scalePixmap(pixmap, w, h):
    return pixmap.scaled(w, h, QtCore.Qt.KeepAspectRatio)

class FilePreview(QWidget):
    def __init__(self):
        super().__init__()

        self._scaleFactor = 1

        self._rootLayout = QVBoxLayout()

        self._previewLabel = QLabel()
        self._previewLabel.setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Ignored)
        self._previewLabel.setScaledContents(True)

        self._scrollArea = QScrollArea()
        self._scrollArea.setBackgroundRole(QPalette.Dark)
        self._scrollArea.setVisible(True)
        self._scrollArea.setAlignment(QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter)
        self._scrollArea.setWidget(self._previewLabel)

        self._rootLayout.addWidget(self._scrollArea)

        self._sizeInfo = QLabel()
        self._rootLayout.addWidget(self._sizeInfo)

        self._dimInfo = QLabel()
        self._rootLayout.addWidget(self._dimInfo)

        self._rootLayout.addStretch()

        self.setLayout(self._rootLayout)

    def _clearPreview(self):
        self._previewLabel.setText("<b>No Preivew</b>")
        self._previewLabel.adjustSize()
        self._sizeInfo.hide()
        self._dimInfo.hide()

    def _setItemForPreview(self, item):
        if item.__class__ != FileNode:
            self._clearPreview()
            return

        path = item.getFullPath()

        fileSize = os.path.getsize(path)
        self._sizeInfo.show()
        self._sizeInfo.setText("<b>Size:</b> {0}".format(_convertSize(fileSize)))

        if item._type == FileNodeType.Image:
            image = QPixmap(path)
            if image.isNull():
                self._dimInfo.hide()
                self._previewLabel.setText("<b>No Preivew</b>")
                Log.error("[FilePreview:_setItemForPreview] Can't load image: '{0}'".format(path))
            else:
                self._dimInfo.show()
                self._dimInfo.setText("<b>Dimension:</b> {0}x{1}".format(image.width(), image.height()))
                self._scaleFactor = 1
                self._previewLabel.setPixmap(
                    _scalePixmap(image, self._scrollArea.width() - 2, self._scrollArea.height() - 2))
                self._previewLabel.adjustSize()
        else:
            self._clearPreview()
