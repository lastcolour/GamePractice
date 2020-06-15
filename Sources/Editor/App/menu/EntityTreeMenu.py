from PyQt5.QtWidgets import QMenu, QAction, QLineEdit

from dialog.SelectChildEntity import SelectChildEntity

class EntityNameEdit(QLineEdit):
    def __init__(self, parent):
        super().__init__(parent)
        self._parent = parent
        self._isRename = False

    def focusOutEvent(self, event):
        if self._isRename:
            self._parent._signal_onRename_lineEdit_focusLost()
        else:
            self._parent._signal_onCreate_lineEdit_focusLost()

class EntityTreeMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)
        self._entityTreeView = parent
        self._currentItem = None
        self._editItem = None

        self.setWindowTitle("Select Child")

        self._addNewChildAct = QAction("Add Child")
        self._addNewChildAct.triggered.connect(self._onAddNewChild)

        self._createNewChildAct = QAction("Create New Child")
        self._createNewChildAct.triggered.connect(self._onCreateNewChild)

        self._renameChildAct = QAction("Rename Child")
        self._renameChildAct.triggered.connect(self._onRenameChild)

        self._extractToEntityAct = QAction("Extract Child To Entity")
        self._extractToEntityAct.triggered.connect(self._onExtractToEntity)

        self._removeChildAct = QAction("Remove Child")
        self._removeChildAct.triggered.connect(self._onRemoveChild)

        self.addAction(self._addNewChildAct)
        self.addAction(self._createNewChildAct)
        self.addAction(self._renameChildAct)
        self.addAction(self._extractToEntityAct)
        self.addAction(self._removeChildAct)

    def _onAddNewChild(self):
        selectDialog = SelectChildEntity(self._currentItem._entity)
        selectDialog.exec()
        res = selectDialog.getResultEntity()
        if res is not None:
            self._entityTreeView._onAddNewChild(self._currentItem, res.getRelativePath())

    def _onRemoveChild(self):
        self._entityTreeView._onRemoveChild(self._currentItem)

    def _onRenameChild(self):
        self._entityNameEdit = EntityNameEdit(self)
        self._entityNameEdit._isRename = True
        self._entityNameEdit.returnPressed.connect(self._signal_onRename_lineEdit_enterEvent)
        self._entityNameEdit._focusLostEvent = self._signal_onRename_lineEdit_focusLost

        self._editItem = self._currentItem
        self._editItem.setText(0, "")
        self._entityTreeView._tree.setItemWidget(self._editItem, 0, self._entityNameEdit)
        self._entityTreeView._tree.setCurrentItem(self._editItem)
        self._entityNameEdit.setFocus(True)
        self._entityNameEdit.setText(self._currentItem._entity.getName())

    def _onCreateNewChild(self):
        self._entityNameEdit = EntityNameEdit(self)
        self._entityNameEdit.returnPressed.connect(self._signal_onCreate_lineEdit_enterEvent)
        self._entityNameEdit._focusLostEvent = self._signal_onCreate_lineEdit_focusLost

        self._editItem = self._entityTreeView._createEditItem(self._currentItem)
        self._entityTreeView._tree.setItemWidget(self._editItem, 0, self._entityNameEdit)
        self._entityTreeView._tree.setCurrentItem(self._editItem)
        self._entityNameEdit.setFocus(True)

    def _onExtractToEntity(self):
        self._entityTreeView._extractEntityToFile(self._currentItem)

    def _signal_onCreate_lineEdit_focusLost(self):
        parentItem = self._editItem.parent()
        if parentItem is None:
            parentItem = self._entityTreeView._tree.invisibleRootItem()
        parentItem.removeChild(self._editItem)
        self._editItem = None
        self._currentItem = None

    def _signal_onRename_lineEdit_focusLost(self):
        self._entityTreeView._tree.setItemWidget(self._currentItem, 0, None)
        self._currentItem.setText(0, self._currentItem._entity.getName())
        self._currentItem = None
        self._editItem = None

    def _signal_onCreate_lineEdit_enterEvent(self):
        childName = self._entityNameEdit.text()
        if self._entityTreeView._createNewChildEntity(self._editItem, childName):
            self._editItem = None
            self._currentItem = None

    def _signal_onRename_lineEdit_enterEvent(self):
        childName = self._entityNameEdit.text()
        if self._entityTreeView._renameChildEntity(self._editItem, childName):
            self._editItem = None
            self._currentItem = None

    def onMenuRequestedOnItem(self, item, pt):
        self._currentItem = item
        if self._currentItem is None:
            return
        if self._currentItem.parent() is None:
            self._removeChildAct.setEnabled(False)
            self._extractToEntityAct.setEnabled(False)
            self._renameChildAct.setEnabled(False)
            self._extractToEntityAct.setEnabled(False)
        else:
            self._removeChildAct.setEnabled(True)
            if self._currentItem._entity.isInternal():
                self._extractToEntityAct.setEnabled(True)
                self._renameChildAct.setEnabled(True)
            else:
                self._renameChildAct.setEnabled(False)
                self._extractToEntityAct.setEnabled(False)
        self.exec(pt)