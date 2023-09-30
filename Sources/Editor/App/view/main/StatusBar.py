from PyQt6.QtWidgets import QStatusBar, QLabel

class StatusBar(QStatusBar):
    def __init__(self):
        super().__init__()

        self._frameResolutionLabel = QLabel()
        self.addWidget(self._frameResolutionLabel)
        self.onOutputFrameSizeChanged(0, 0)

    def onOutputFrameSizeChanged(self, width, height):
        self._frameResolutionLabel.setText("Frame: {0}x{1}".format(width, height))