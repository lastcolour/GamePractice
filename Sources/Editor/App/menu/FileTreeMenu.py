from PyQt5.QtWidgets import QMenu, QAction, QLineEdit, QMessageBox

from dialog.RemoveFile import RemoveFile
from utils.ViewUtils import OpenPlatformFileExplorer
from utils.Managers import GetEventManager

class FileNameEdit(QLineEdit):
    def __init__(self, parent):
        super().__init__(parent)
        self._parent = parent
        self._isDir = False
        self._isRename = False

    def focusOutEvent(self, event):
        if self._isRename:
            self._parent._signal_onRename_lineEdit_focusLost()
        else:
            self._parent._signal_onCreate_lineEdit_focusLost()

class FileTreeMenu(QMenu):
    def __init__(self, parent):
        super().__init__(parent)

        self._fileTreeView = parent
        self._currentItem = None
        self._copyItem = None
        self._cutItem = None

        self._newDirectoryAct = QAction("New Directoty")
        self._newDirectoryAct.triggered.connect(self._onCreateNewDir)

        self._newEntityAct = QAction("New Entity")
        self._newEntityAct.triggered.connect(self._onCreateNewEntity)

        self._copyRelativePathAct = QAction("Copy Relative Path")
        self._copyRelativePathAct.triggered.connect(self._onCopyRelativePath)

        self._renameAct = QAction("Rename")
        self._renameAct.triggered.connect(self._onRename)

        self._copyAct = QAction("Copy")
        self._copyAct.triggered.connect(self._onCopy)

        self._cutAct = QAction("Cut")
        self._cutAct.triggered.connect(self._onCut)

        self._pasteAct = QAction("Paste")
        self._pasteAct.triggered.connect(self._onPaste)

        self._removeAct = QAction("Remove")
        self._removeAct.triggered.connect(self._onRemove)

        self._showInExplorerAct = QAction("Show in Explorer")
        self._showInExplorerAct.triggered.connect(self._onShowInExplorer)

        self._reloadModelAct = QAction("Reload")
        self._reloadModelAct.triggered.connect(self._onReloadModel)

        self.addAction(self._newDirectoryAct)
        self.addAction(self._newEntityAct)
        self.addAction(self._copyRelativePathAct)
        self.addAction(self._renameAct)
        self.addAction(self._copyAct)
        self.addAction(self._cutAct)
        self.addAction(self._pasteAct)
        self.addAction(self._removeAct)
        self.addAction(self._showInExplorerAct)
        self.addAction(self._reloadModelAct)

    def onMenuRequestedOnItem(self, item, pt):
        if item is None:
            self._copyAct.setEnabled(False)
            self._cutAct.setEnabled(False)
            self._removeAct.setEnabled(False)
            self._renameAct.setEnabled(False)
            self._copyRelativePathAct.setEnabled(False)
        else:
            self._copyAct.setEnabled(True)
            self._cutAct.setEnabled(True)
            self._removeAct.setEnabled(True)
            self._renameAct.setEnabled(True)
            self._copyRelativePathAct.setEnabled(True)
        if self._copyItem is None and self._cutItem is None:
            self._pasteAct.setEnabled(False)
        else:
            self._pasteAct.setEnabled(True)
        self._currentItem = item
        self.exec(pt)

    def _onCreateNewDir(self):
        self._fileNameEdit = FileNameEdit(self)
        self._fileNameEdit._isDir = True
        self._fileNameEdit.returnPressed.connect(self._signal_onCreate_lineEdit_enterEvent)
        self._fileNameEdit._focusLostEvent = self._signal_onCreate_lineEdit_focusLost

        self._editItem = self._fileTreeView._createEditDirItem(self._currentItem)
        self._fileTreeView._tree.setItemWidget(self._editItem, 0, self._fileNameEdit)
        self._fileTreeView._tree.setCurrentItem(self._editItem)
        self._fileNameEdit.setFocus(True)

    def _onCreateNewEntity(self):
        self._fileNameEdit = FileNameEdit(self)
        self._fileNameEdit._isDir = False
        self._fileNameEdit.returnPressed.connect(self._signal_onCreate_lineEdit_enterEvent)
        self._fileNameEdit._focusLostEvent = self._signal_onCreate_lineEdit_focusLost

        self._editItem = self._fileTreeView._createEditFileItem(self._currentItem)
        self._fileTreeView._tree.setItemWidget(self._editItem, 0, self._fileNameEdit)
        self._fileTreeView._tree.setCurrentItem(self._editItem)
        self._fileNameEdit.setFocus(True)

    def _onRename(self):
        self._fileNameEdit = FileNameEdit(self)
        self._fileNameEdit._isRename = True
        self._fileNameEdit.returnPressed.connect(self._signal_onRename_lineEdit_enterEvent)

        self._fileTreeView._tree.setItemWidget(self._currentItem, 0, self._fileNameEdit)
        self._fileTreeView._tree.setCurrentItem(self._currentItem)
        self._fileNameEdit.setFocus(True)
        self._currentItem.setText(0, "")
        self._fileNameEdit.setText(self._currentItem._node.getBaseName())

    def _onCopy(self):
        pass

    def _onCut(self):
        pass

    def _onPaste(self):
        pass

    def _onCopyRelativePath(self):
        pass

    def _onRemove(self):
        res = RemoveFile(self._currentItem._node).exec()
        if res != QMessageBox.Ok:
            return
        self._fileTreeView._removeItem(self._currentItem)
        self._currentItem = None

    def _onShowInExplorer(self):
        if self._currentItem is None:
            filePath = self._fileTreeView._getRootFileNode()
        else:
            filePath = self._currentItem._node.getFullPath()
        OpenPlatformFileExplorer(filePath)

    def _signal_onRename_lineEdit_focusLost(self):
        self._fileTreeView._tree.setItemWidget(self._currentItem, 0, None)
        self._currentItem.setText(0, self._currentItem._node.getBaseName())
        self._currentItem = None

    def _signal_onRename_lineEdit_enterEvent(self):
        fileName = self._fileNameEdit.text()
        if self._fileTreeView._renameItem(self._currentItem, fileName):
            self._currentItem = None

    def _signal_onCreate_lineEdit_enterEvent(self):
        fileName = self._fileNameEdit.text()
        if self._fileNameEdit._isDir:
            res = self._fileTreeView._createNewDir(self._editItem, fileName)
        else:
            res = self._fileTreeView._createNewEntity(self._editItem, fileName)
        if res:
            self._editItem = None
            self._currentItem = None

    def _signal_onCreate_lineEdit_focusLost(self):
        parentItem = self._editItem.parent()
        if parentItem is None:
            parentItem = self._fileTreeView._tree.invisibleRootItem()
        parentItem.removeChild(self._editItem)
        self._editItem = None
        self._currentItem = None

    def _onReloadModel(self):
        GetEventManager().rebuildAssetsModel()