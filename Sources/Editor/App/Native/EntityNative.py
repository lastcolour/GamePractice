from .Native import NativeObject
from .LogicNative import CreateLogic

class EntityNative(NativeObject):
    def __init__(self):
        self._name = None
        self._entityId = None
        self._children = []
        self._logics = []

    def isLoadedToNative(self):
        return self._entityId is not None

    def _createEntityLogicId(self):
        if len(self._logics) == 0:
            return 0
        else:
            return self._logics[-1]._logicId + 1

    def _syncWithNative(self):
        childIds = self._getAPI().getLibrary().getEntityChildren(self._entityId)
        if len(self._children) != len(childIds):
            raise RuntimeError("Invalid number of children")
        for i in range(len(self._children)):
            self._children[i]._entityId = childIds[i]
            self._children[i]._syncWithNative()

    def loadToNative(self):
        self._entityId = self._getAPI().getLibrary().loadEntity(self._name)
        if self._entityId == 0:
            print("[EntityNative:loadToNative] Can't load entity '{0}' to editor", self._name)
            return False
        self._syncWithNative()
        return True

    def addLogic(self, logicName):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't add logic '{0}' to entity that is not loaded to edit: '{1}'".format(
                logicName, self._name))
        logicId = self._getAPI().getLibrary().addLogicToEntity(self._entityId, logicName)
        if logicId == -1:
            print("[EntityNative:addLogic] Can't create native part of logic '{0}' for entity {1}".format(
                logicName, self._name))
            return None
        logic = CreateLogic(logicName)
        if logic is None:
            print("[EntityNative:addLogic] Can't add '{0}' logic to entity '{1}'".format(logicName, self._name))
            return None
        logic._logicId = logicId
        logic._entity = self
        logic.readFromNative()
        self._logics.append(logic)
        return logic

    def addLogicWithData(self, logicName, logicData):
        if self.isLoadedToNative():
            raise RuntimeError("Can't add logic '{0}' with data to entity that is loaded to editor: '{1}'".format(
                logicName, self._name))
        logic = CreateLogic(logicName)
        if logic is None:
            print("[EntityNative:addLogicWithData] Can't add '{0}' logic to entity '{1}'".format(logicName, self._name))
            return None
        logic._logicId = self._createEntityLogicId()
        logic._entity = self
        logic._rootValue.readFromDict(logicData)
        self._logics.append(logic)
        return logic

    def removeLogic(self, logicId):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't remove logic from entity that isn't loaded to native: '{0}'", self._name)
        logicToRemove = None
        for logic in self._logics:
            if logic.getNativeId() == logicId:
                logicToRemove = logic
        if logicToRemove is None:
            print("[EntityNative:removeLogic] Can't find logic to remove with id '{0}' from entity {1}".format(
                logicId, self._name))
            return
        self._getAPI().getLibrary().removeLogicFromEntity(self._entityId, logicId)
        self._logics.remove(logicToRemove)

    def getLogics(self):
        return self._logics
    
    def getChildren(self):
        return self._children

    def getNativeId(self):
        return self._entityId