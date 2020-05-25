from PyQt5.QtWidgets import QWidget, QPushButton, QVBoxLayout, QFrame

class EntityLogicsView(QWidget):
    def __init__(self):
        super().__init__()

        self._rootLayout = QVBoxLayout()

        self._addLogicBt = QPushButton("Add Logic")
        self._rootLayout.addWidget(self._addLogicBt)

        self._frame = QFrame()
        self._frameLayout = QVBoxLayout()
        self._frame.setLayout(self._frameLayout)
        self._rootLayout.addWidget(self._frame)

        self.setLayout(self._rootLayout)