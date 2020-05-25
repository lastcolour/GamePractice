from PyQt5.QtWidgets import QWidget, QTreeWidget, QVBoxLayout

class EntityTreeView(QWidget):
    def __init__(self):
        super().__init__()

        self._rootLayout = QVBoxLayout()

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._rootLayout.addWidget(self._tree)

        self.setLayout(self._rootLayout)