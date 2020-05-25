from PyQt5.QtWidgets import QWidget, QTreeWidget, QLineEdit, QVBoxLayout

class EntityFileView(QWidget):
    def __init__(self):
        super().__init__()

        self._rootLayout = QVBoxLayout()

        self._search = QLineEdit()
        self._search.setPlaceholderText("Search...")
        self._rootLayout.addWidget(self._search)

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._rootLayout.addWidget(self._tree)

        self.setLayout(self._rootLayout)