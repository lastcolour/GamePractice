from utils.Log import Log

from dialog.SaveEntityChanges import SaveEntityChanges
from dialog.OverrideFile import OverrideFile

from PyQt6.QtWidgets import QMessageBox, QFileDialog

from msg.Messages import MsgOpenEntityForEdit, MsgSetEditEntity, \
    MsgOnAddLogicBtPressed, MsgAddLogicToEntity
from msg.MessageSystem import RegisterForMessage, SendMessage

import os
import traceback

class EventManager:

    def __init__(self, mgr):
        self._mgr = mgr
        self._app = mgr._app
        self._currentEntity = None
        RegisterForMessage(MsgOpenEntityForEdit, self._onOpenNewEditEntity)
        RegisterForMessage(MsgOnAddLogicBtPressed, self._onAddLogicToEntity)

    def _askToSaveEntity(self):
        if not self._currentEntity.isModified():
            return True
        retCode = SaveEntityChanges(self._currentEntity).exec_()
        if retCode == QMessageBox.Ok:
            self._currentEntity.save()
        elif retCode == QMessageBox.Cancel:
            return False
        return True

    def _onOpenNewEditEntity(self, msg):
        if self._currentEntity is not None:
            if self._currentEntity.getName() == msg.entityName:
                return
            if not self._askToSaveEntity():
                return
        loader = self._app._editorNative.getEntityLoader()
        try:
            newEntity = loader.loadEntity(msg.entityName)
        except Exception as e:
            Log.error("[_EventManager:_onOpenNewEditEntity] Can't load entity '{0}' (Exception: {1})".format(
                msg.entityName, e))
            traceback.print_tb(e.__traceback__)
            return
        if newEntity is None:
            return
        if not newEntity.loadToNative():
            Log.error("[_EventManager:_onOpenNewEditEntity] Can't load entity '{0}' to native".format(
                msg.entityName))
            return
        if self._currentEntity is not None:
            self._currentEntity.unloadFromNative()
        self._currentEntity = newEntity
        SendMessage(MsgSetEditEntity(self._currentEntity))

    def _onAddLogicToEntity(self, msg):
        entity = msg.entity
        if entity is None:
            raise RuntimeError("Can't add logic to invalid entity")
        if not entity.isLoadedToNative():
            raise RuntimeError("Can't add logic to entity '{0}' that is not loaded to native".format(
                entity.getName()))
        from dialog.UniversalSelectDialog import ExecEntityLogicSelectDialog
        logic = ExecEntityLogicSelectDialog()
        if logic is None:
            return
        entityLogic = entity.addLogic(logic.getName())
        if entityLogic is None:
            raise RuntimeError("Can't add logic '{0}' to  entity '{1}'".format(logic.getName(), entity.getName()))
        SendMessage(MsgAddLogicToEntity(entity, entityLogic))

    def onAddCopyLogic(self, editEntity, logicName, logicData):
        if editEntity is None:
            raise RuntimeError("Can't add logic to invalid entity")
        if not editEntity.isLoadedToNative():
            raise RuntimeError("Can't add logic to entity '{0}' that is not loaded to native".format(
                editEntity.getName()))
        entityLogic = editEntity.addCopyLogic(logicName, logicData)
        if entityLogic is None:
            raise RuntimeError("Can't add logic '{0}' to entity '{1}'".format(logicName, editEntity.getName()))
        SendMessage(MsgAddLogicToEntity(editEntity, entityLogic))

    def onRemoveEntityLogicBtClicked(self, editLogic):
        editLogic.getEntity().removeLogic(editLogic.getNativeId())
        self._app._entityLogicsView.removeLogicView(editLogic.getNativeId())

    def onChildEntityExtractToFile(self, childEntity):
        name = childEntity.getFullFilePath()
        if name[-5:] != '.json':
            name += ".json"
        resFile = QFileDialog.getSaveFileName(self._app, "", "{0}".format(name), "Entity (*.json)")
        resFile = resFile[0]
        if resFile is None:
            return False
        if os.path.exists(resFile):
            if os.path.isdir(resFile):
                return False
            retCode = OverrideFile(resFile).exec_()
            if retCode != QMessageBox.Ok:
                return False
        if not childEntity.extractToFile(resFile):
            return False
        self._app._assetsModel.reload()
        self._app._entityFileView.setFileTreeModel(self._app._assetsModel.getEntitiesTree())
        return True

    def onAddCopyChild(self, entity, copyEntitydata):
        if entity is None:
            raise RuntimeError("Can't add logic to invalid entity")
        if not entity.isLoadedToNative():
            raise RuntimeError("Can't add logic to entity '{0}' that is not loaded to native".format(
                entity.getName()))
        if copyEntitydata._isInternal:
            copyEntity = entity.addChildEntityFromData(copyEntitydata._name, copyEntitydata._data)
        else:
            copyEntity = entity.addChildEntity(copyEntitydata._name)
        return copyEntity

    def rebuildAssetsModel(self):
        self._app._assetsModel.reload()
        self._app._entityFileView.setFileTreeModel(self._app._assetsModel.getEntitiesTree())

    def hasEditEntity(self):
        return self._currentEntity is not None

    def closeEditEntity(self):
        if self._currentEntity is not None:
            if not self._askToSaveEntity():
                return
            self._currentEntity.unloadFromNative()
        self._currentEntity = None
        SendMessage(MsgSetEditEntity(self._currentEntity))

    def saveEditEntity(self):
        self._currentEntity.save()

    def getAssetsModel(self):
        return self._app._assetsModel

    def getSoundEventsModel(self):
        return self._app._soundEventsModel

    def getEntityLogicsModel(self):
        return self._app._logicsModel