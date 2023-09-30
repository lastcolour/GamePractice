from PyQt6.QtWidgets import QToolBar, QPushButton, QComboBox, QLabel

from utils.Managers import GetEngineViewManager
from utils.Managers import GetGameManager

from view.EngineOutputFrameView import AspecRatio

class MainToolBar(QToolBar):
    def __init__(self):
        super().__init__()

        self._gameUpateBt = QPushButton("Enable Updates")
        self._gameUpateBt.setCheckable(True)
        self._gameUpateBt.clicked.connect(self._signal_gameUpdateBt_clicked)
        self.addWidget(self._gameUpateBt)

        self._timeScaleLabel = QLabel("Time Scale:")
        self.addWidget(self._timeScaleLabel)

        self._timeScaleBox = QComboBox()
        self._timeScaleBox.addItems([
            "25%",
            "50%",
            "75%",
            "100%",
            "125%",
            "150%",
            "175%",
            "200%",
        ])
        self._timeScaleBox.setCurrentIndex(self._timeScaleBox.findText("100%"))
        self._timeScaleBox.currentTextChanged.connect(self._signal_timeScaleBox_changed)
        self.addWidget(self._timeScaleBox)

        self.addSeparator()

        self._orientationLabel = QLabel("Orientation:")
        self.addWidget(self._orientationLabel)

        self._orientationBox = QComboBox()
        self._orientationBox.addItems([
            "Horizontal",
            "Vertical"
        ])
        self._orientationBox.setCurrentIndex(self._orientationBox.findText("Vertical"))
        self._orientationBox.currentTextChanged.connect(self._signal_orientationBox_changed)
        self.addWidget(self._orientationBox)

        self._aspectRationLabel = QLabel("Aspect Ratio:")
        self.addWidget(self._aspectRationLabel)

        self._aspectRationBox = QComboBox()
        self._aspectRationBox.addItems([
            "4x3",
            "5x3",
            "16x9",
            "16x10",
            "18x9",
            "21x9",
            "None"
        ])
        self._aspectRationBox.setCurrentIndex(self._aspectRationBox.findText("None"))
        self._aspectRationBox.currentTextChanged.connect(self._singnal_aspectRationBox_changed)
        self.addWidget(self._aspectRationBox)

        self.addSeparator()

        self._startGameBt = QPushButton("Start Game")
        self._startGameBt.clicked.connect(self._signal_startGameBt_clicked)
        self.addWidget(self._startGameBt)

        self.setMovable(False)
        self.layout().setSpacing(5)

    def _signal_gameUpdateBt_clicked(self):
        GetEngineViewManager().onEnableGameUpdate(self._gameUpateBt.isChecked())

    def _signal_timeScaleBox_changed(self, text):
        timeScale = float(text[:-1]) / 100
        GetEngineViewManager().onTimeScaleChanged(timeScale)

    def _signal_orientationBox_changed(self, text):
        isHorizontal = False
        if text == "Horizontal":
            isHorizontal = True
        GetEngineViewManager().onOrientationChanged(isHorizontal)

    def _singnal_aspectRationBox_changed(self, text):
        aspectRation = None
        if text == "4x3":
            aspectRation = AspecRatio.Ratio_4x3
        elif text == "5x3":
            aspectRation = AspecRatio.Ratio_5x3
        elif text == "16x9":
            aspectRation = AspecRatio.Ratio_16x9
        elif text == "16x10":
            aspectRation = AspecRatio.Ratio_16x10
        elif text == "18x9":
            aspectRation = AspecRatio.Ratio_18x9
        elif text == "21x9":
            aspectRation = AspecRatio.Ratio_21x9
        elif text == "None":
            aspectRation = AspecRatio.Ratio_None
        else:
            raise RuntimeError("Unknown aspect ration: '{0}'".format(text))
        GetEngineViewManager().onAspectRatioChanged(aspectRation)

    def _signal_startGameBt_clicked(self):
        GetGameManager().startGame()