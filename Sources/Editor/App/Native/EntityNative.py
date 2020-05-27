from .Native import NativeObject
from .LogicNative import CreateLogic

class EntityNative(NativeObject):
    def __init__(self):
        self._isModified = False
        self._name = None
        self._entityId = None
        self._children = []
        self._logics = []
        self._parent = None

    def getName(self):
        return self._name

    def isModified(self):
        for child in self._children:
            if child.isModified():
                return True
        for logic in self._logics:
            if logic.isModified():
                return True
        return self._isModified

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
        self._isModified = True
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
            print("[EntityNative:removeLogic] Can't find logic to remove with id '{0}' from entity '{1}'".format(
                logicId, self._name))
            return
        self._getAPI().getLibrary().removeLogicFromEntity(self._entityId, logicId)
        self._logics.remove(logicToRemove)
        self._isModified = True

    def getLogics(self):
        return self._logics
    
    def getChildren(self):
        return self._children

    def getNativeId(self):
        return self._entityId

    def addChildEntity(self, entityName):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't add child '{0}' entity to entity '{1}' that is not loaded to edit".format(
                entityName, self._name))
        childEntityId = self._getAPI().getLibrary().addChildEntityToEntity(self._entityId, entityName)
        if childEntityId == 0:
            print("[EntityNative:addChildEntity] Can't add child entity '{0}' to '{1}'".format(entityName, self._name))
            return None
        childEntity = self._getAPI().getEntityLoader().loadEntity(entityName)
        if childEntity is None:
            print("[EntityNative:addChildEntity] Can't load an entity '{0}' to as a child to to '{1}'".format(entityName, self._name))
            return None
        childEntity._parent = self
        childEntity._entityId = childEntityId
        self._children.append(childEntity)
        self._isModified = True
        return childEntity

    def removeChildEntity(self, childEntityId):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't remove child entity with id '{0}' from entity '{1}' that isn't loaded to edit".format(
                childEntityId, self._name))
        childToRemove = None
        for child in self._children:
            if child.getNativeId() == childEntityId:
                childToRemove = child
                break
        if childToRemove is None:
            print("[EntityNative:removeChildEntity] Can't find child with id '{0}' to remove from entity {1}".format(
                childEntityId, self._name))
            return
        self._getAPI().getLibrary().removeChildEntityFromEntity(self._entityId, childEntityId)
        self._children.remove(childToRemove)
        self._isModified = True
