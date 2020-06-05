from PyQt5.QtWidgets import QMenu, QAction

from dialog.SelectChildEntity import SelectChildEntity

class EntityTreeMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)
        self._entityTreeView = parent
        self._currentItem = None

        self.setWindowTitle("Select Child")

        self._addNewChildAct = QAction("Add Child")
        self._addNewChildAct.triggered.connect(self._onAddNewChild)

        self._removeChildAct = QAction("Remove Child")
        self._removeChildAct.triggered.connect(self._onRemoveChild)

        self.addAction(self._addNewChildAct)
        self.addAction(self._removeChildAct)

    def _onAddNewChild(self):
        selectDialog = SelectChildEntity(self._currentItem._entity)
        selectDialog.exec()
        res = selectDialog.getResultEntity()
        if res is not None:
            self._entityTreeView._onAddNewChild(self._currentItem, res.getRelativePath())

    def _onRemoveChild(self):
        self._entityTreeView._onRemoveChild(self._currentItem)

    def onMenuRequestedOnItem(self, item, pt):
        self._currentItem = item
        if self._currentItem is None:
            return
        if self._currentItem.parent() is None:
            self._removeChildAct.setEnabled(False)
        else:
            self._removeChildAct.setEnabled(True)
        self.exec(pt)