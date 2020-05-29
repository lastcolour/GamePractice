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

    def onEntityDoubleClickFromFileTree(self, entityName):
        if self._currentEntity is not None:
            if self._currentEntity.getName() == entityName:
                return
            if self._currentEntity.isModified():
                retCode = SaveEntityChanges(self._currentEntity).exec_()
                if retCode == QMessageBox.Ok:
                    self._currentEntity.save()
                elif retCode == QMessageBox.Cancel:
                    return
                elif retCode == QMessageBox.No:
                    pass
        loader = self._app._editorNative.getEntityLoader()
        newEntity = loader.loadEntity(entityName)
        if newEntity is None:
            return
        if not newEntity.loadToNative():
            Log.error("[_EventManager:onEntityDoubleClickFromFileTree] Can't load entity '{0}' to native".format(
                entityName))
            return
        self._currentEntity = newEntity
        self._app._entityLogicsView.widget().setEditEntity(self._currentEntity)

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

def GetEventManager():
    return _EventManager._INSTANCE

def CreateEventManager(app):
    manger = _EventManager(app)
    _EventManager._INSTANCE = manger