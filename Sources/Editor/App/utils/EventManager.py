from utils.Log import Log
from dialog.LogicSelecDialog import LogicSelectDialog
from dialog.SaveEntityChanges import SaveEntityChanges
from dialog.RemoveEntityLogic import RemoveEntityLogic

from PyQt5.QtWidgets import QMessageBox

class _EventManager:

    _INSTANCE = None

    def __init__(self, app):
        if _EventManager._INSTANCE is not None:
            raise RuntimeError("One instance of event manager already exists")
        self._app = app
        self._currentEntity = None
        self._app._entityFileView.widget().setFileTreeModel(self._app._assetsModel)

    def _askToSaveEntity(self):
        if not self._currentEntity.isModified():
            return True
        retCode = SaveEntityChanges(self._currentEntity).exec_()
        if retCode == QMessageBox.Ok:
            self._currentEntity.save()
        elif retCode == QMessageBox.Cancel:
            return False
        return True

    def onEntityDoubleClickFromFileTree(self, entityName):
        if self._currentEntity is not None:
            if self._currentEntity.getName() == entityName:
                return
            if not self._askToSaveEntity():
                return
        loader = self._app._editorNative.getEntityLoader()
        try:
            newEntity = loader.loadEntity(entityName)
        except Exception as e:
            Log.error("[_EventManager:onEntityDoubleClickFromFileTree] Can't load entity '{0}' (Exception: {1})".format(
                entityName, e))
            return
        if newEntity is None:
            return
        if not newEntity.loadToNative():
            Log.error("[_EventManager:onEntityDoubleClickFromFileTree] Can't load entity '{0}' to native".format(
                entityName))
            return
        if self._currentEntity is not None:
            self._currentEntity.unloadFromNative()
        self._currentEntity = newEntity
        self._app._entityLogicsView.widget().setEditEntity(self._currentEntity)
        self._app._entityTreeView.widget().setEditEntity(self._currentEntity)

    def onEntityClickedFromEntityTree(self, entity):
        self._app._entityLogicsView.widget().setEditEntity(entity)

    def onAddLogicBtClicked(self, editEntity):
        if editEntity is None:
            raise RuntimeError("Can't add logic to invlid entity")
        if not editEntity.isLoadedToNative():
            raise RuntimeError("Can't add logic to entity '{0}' that is not loaded to native".format(
                editEntity.getName()))
        selectDialog = LogicSelectDialog(self._app._logicsModel)
        selectDialog.exec_()
        logic = selectDialog.getResultLogic()
        if logic is None:
            return
        entityLogic = editEntity.addLogic(logic.getName())
        if entityLogic is None:
            raise RuntimeError("Can't add logic '{0}' to  entity '{1}'".format(logic.getName(), editEntity.getName()))
        self._app._entityLogicsView.widget().addLogicView(entityLogic)

    def onRemoveEntityLogicBtClicked(self, editLogic):
        retCode = RemoveEntityLogic(editLogic).exec_()
        if retCode == QMessageBox.Ok:
            editLogic.getEntity().removeLogic(editLogic.getNativeId())
            self._app._entityLogicsView.widget().removeLogicView(editLogic.getNativeId())
        elif retCode == QMessageBox.Cancel:
            pass
        elif retCode == QMessageBox.No:
            pass

    def drawNativeFrameTo(self, ptr, width, height):
        self._app._editorNative.getLibrary().drawFrame(ptr, width, height)

    def getAssetsModel(self):
        return self._app._assetsModel

    def hasEditEntity(self):
        return self._currentEntity is not None

    def closeEditEntity(self):
        if self._currentEntity is not None:
            if not self._askToSaveEntity():
                return
            self._currentEntity.unloadFromNative()
        self._currentEntity = None
        self._app._entityLogicsView.widget().setEditEntity(None)
        self._app._entityTreeView.widget().setEditEntity(None)

    def saveEditEntity(self):
        self._currentEntity.save()

def GetEventManager():
    return _EventManager._INSTANCE

def CreateEventManager(app):
    manger = _EventManager(app)
    _EventManager._INSTANCE = manger