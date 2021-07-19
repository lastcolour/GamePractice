from .Native import NativeObject
from .LogicNative import CreateLogic

import traceback

def _getUniqueNameForNewChild(entity, entityName):
    childEntityName = entityName
    isNameUnique = False
    idx = 1
    while not isNameUnique:
        isNameUnique = True
        for child in entity._children:
            if child._name == childEntityName:
                childEntityName = "{0} ({1})".format(entityName, idx)
                idx += 1
                isNameUnique = False
                break
    return childEntityName

class EntityNative(NativeObject):
    def __init__(self):
        self._isModified = False
        self._name = None
        self._entityId = None
        self._tmLogic = self._createTransformLogic()
        self._children = []
        self._logics = []
        self._parent = None
        self._nameSuffix = None
        self._childId = None
        self._isInternal = False
        self._doNotSyncFlag = True

    def getName(self):
        return self._name

    def getNameSuffix(self):
        return self._nameSuffix

    def getParent(self):
        return self._parent

    def setDoNotSync(self, doNotSyncFlag):
        self._doNotSyncFlag = doNotSyncFlag

    def shouldSyncWithNative(self):
        return self._doNotSyncFlag == False

    def isModified(self):
        if self._isModified:
            return True
        if self._tmLogic.isModified():
            return True
        for logic in self._logics:
            if logic.isModified():
                return True
        for child in self._children:
            if child.isModified():
                return True
        return False

    def isLoadedToNative(self):
        return self._entityId is not None

    def _syncWithNative(self):
        for childEnt in self._children:
            childEnt._entityId = self._getAPI().getLibrary().getEntityChildEntityId(self._entityId, childEnt._childId)
            childEnt._syncWithNative()
        self._tmLogic.readFromNative()
        for logic in self._logics:
            logic.readFromNative()

    def _desyncWithNative(self):
        self._entityId = None
        for child in self._children:
            child._desyncWithNative()

    def loadToNative(self):
        if self._isInternal:
            raise RuntimeError("Can't load internal entity '{0}' to native".format(self._name))
        self._entityId = self._getAPI().getLibrary().loadEntityFromFile(self._name)
        if self._entityId == 0:
            print("[EntityNative:loadToNative] Can't load entity '{0}' to editor".format(self._name))
            return False
        self._syncWithNative()
        return True

    def unloadFromNative(self):
        self._getAPI().getLibrary().unloadEntity(self._entityId)
        self._desyncWithNative()

    def setInFocus(self):
        self._getAPI().getLibrary().setFocusEntity(self._entityId)

    def addLogic(self, logicName):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't add logic '{0}' to entity that is not loaded to edit: '{1}'".format(
                logicName, self._name))
        logicId = self._getAPI().getLibrary().addLogicToEntity(self._entityId, logicName)
        if logicId == -1:
            print("[EntityNative:addLogic] Can't create native part of logic '{0}' for entity '{1}'".format(
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

    def addCopyLogic(self, logicName, logicData):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't add logic '{0}' to entity that is not loaded to edit: '{1}'".format(
                logicName, self._name))
        logicId = self._getAPI().getLibrary().addLogicToEntity(self._entityId, logicName)
        if logicId == -1:
            print("[EntityNative:addLogic] Can't create native part of logic '{0}' for entity '{1}'".format(
                logicName, self._name))
            return None
        logic = CreateLogic(logicName)
        if logic is None:
            print("[EntityNative:addLogic] Can't add '{0}' logic to entity '{1}'".format(logicName, self._name))
            return None
        logic._logicId = logicId
        logic._entity = self
        logic.readFromCopyDict(logicData)
        self._logics.append(logic)
        self._isModified = True
        return logic

    def addLogicWithData(self, logicName, logicId, logicData):
        if self.isLoadedToNative():
            raise RuntimeError("Can't add logic '{0}' with data to entity that is loaded to editor: '{1}'".format(
                logicName, self._name))
        logic = CreateLogic(logicName)
        if logic is None:
            print("[EntityNative:addLogicWithData] Can't add '{0}' logic to entity '{1}'".format(logicName, self._name))
            return None
        logic._logicId = logicId
        logic._entity = self
        try:
            logic._rootValue.readFromDict(logicData)
        except Exception as e:
            print("[EntityNative:addLogicWithData] Can't read data of logic '{0}' of entity entity '{1}' (Error: {2})".format(
                logicName, self._name, e))
            traceback.print_tb(e.__traceback__)
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
        childEntity = self._getAPI().getEntityLoader().loadEntity(entityName)
        if childEntity is None:
            print("[EntityNative:addChildEntity] Can't load entity form file '{0}' to add as a child to '{1}'".format(entityName, self._name))
            return None
        if not childEntity.loadToNative():
            print("[EntityNative:addChildEntity] Can't load entity '{0}' to native to add as a child to '{1}'".format(entityName, self._name))
            return None
        childEntity._childId = self._getAPI().getLibrary().addChildEntityToEntity(self._entityId, childEntity._entityId)
        if childEntity._childId == -1:
            childEntity.unloadFromNative()
            print("[EntityNative:addChildEntity] Can't add child entity '{0}' to entity '{1}'".format(entityName, self._name))
            return None
        childEntity._parent = self
        duplicatesCount = 0
        for entity in self._children:
            if entity.getName() == entityName:
                duplicatesCount += 1
        childEntity._nameSuffix = "({0})".format(duplicatesCount)
        self._children.append(childEntity)
        self._isModified = True
        return childEntity

    def removeChildEntity(self, childEntity):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't remove child entity with id '{0}' from entity '{1}' that isn't loaded to edit".format(
                childEntity._name, self._name))
        childToRemove = None
        for child in self._children:
            if child.getNativeId() == childEntity.getNativeId():
                childToRemove = child
                break
        if childToRemove is None:
            print("[EntityNative:removeChildEntity] Can't find child '{0}' with id '{1}' to remove from entity {2}".format(
                childEntity._name, childEntity.getNativeId(), self._name))
            return
        self._getAPI().getLibrary().removeChildEntityFromEntity(self._entityId, childEntity.getNativeId())
        self._children.remove(childToRemove)
        self._isModified = True

    def addChildEntityFromData(self, entityName, entityData):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't add child '{0}' entity to entity '{1}' that is not loaded to edit".format(
                entityName, self._name))
        uniqueChildName = _getUniqueNameForNewChild(self, entityName)
        childEntity = self._getAPI().getEntityLoader()._loadEntityFromData(uniqueChildName, entityData)
        if childEntity is None:
            print("[EntityNative:addChildEntityFromData] Can't load entity form data to add as a child to '{0}'".format(self._name))
            return None
        childEntity._entityId = self._getAPI().getLibrary().loadEntityFromData(uniqueChildName, entityData)
        if childEntity._entityId is None:
            print("[EntityNative:addChildEntityFromData] Can't load entity to native to add as a child to '{0}'".format(self._name))
            return None
        childEntity._childId = self._getAPI().getLibrary().addChildEntityToEntity(self._entityId, childEntity._entityId)
        if childEntity._childId == -1:
            print("[EntityNative:addChildEntityFromData] Can't add child entity '{0}' from data to entity '{1}'".format(entityName, self._name))
            childEntity.unloadFromNative()
            return None
        childEntity._parent = self
        childEntity._isInternal = True
        childEntity._isModified = True
        self._getAPI().getEntityLoader().setupDefaultTransform(childEntity)
        childEntity._tmLogic.writeToNative()
        self._children.append(childEntity)
        self._isModified = True
        self._syncWithNative()
        return childEntity

    def getFullFilePath(self):
        return self._getAPI().getEntityLoader().getEntityFullPath(self._name)

    def _dumpChildToDict(self, child):
        tmRes = {}
        child._tmLogic.writeToDict(tmRes)
        data = {
            "transform": tmRes["data"],
            "name": child.getName(),
            "id": child._childId,
            "internal":child.isInternal()
        }
        if child.isInternal():
            data["data"] = child.dumpToDict()
        return data

    def dumpToDict(self):
        res = {"children":[], "logics":[]}
        for child in self._children:
            res["children"].append(self._dumpChildToDict(child))
        for logic in self._logics:
            logicRes = {}
            logic.writeToDict(logicRes)
            res["logics"].append(logicRes)
        return res

    def getTransformLogic(self):
        return self._tmLogic

    def save(self):
        self._isModified = False
        if self._isInternal:
            return
        self._getAPI().getEntityLoader().saveEntity(self)

    def canAddChild(self, entityName):
        if self._name == entityName:
            return False
        if self._parent is not None:
            return self._parent.canAddChild(entityName)
        return True

    def getChildWithId(self, childId):
        if childId == -1:
            return None
        if childId == 0:
            return self
        for child in self._children:
            if child._childId == childId:
                return child
        print("[EntityNative:getChildWithId] Can't find child with id '{0}' in entity: '{1}'".format(
            childId, self._name))
        return None

    def isInternal(self):
        return self._isInternal

    def _createTransformLogic(self):
        tmLogic = CreateLogic("Transform")
        tmLogic._logicId = 0
        tmLogic._entity = self
        return tmLogic

    def rename(self, newName):
        if not self._isInternal:
            raise RuntimeError("Can't rename non-internal child")
        if not self.isLoadedToNative():
            raise RuntimeError("Entity is't loaded to native")
        if self._name == newName:
            return True
        for child in self._parent._children:
            if child._name == newName:
                return False
        if not self._getAPI().getLibrary().renameEntity(self._entityId, newName):
            print("[EntityNative:rename] Can't rename {0} to {1}".format(self._name, newName))
            return False
        self._name = newName
        return True

    def extractToFile(self, resFile):
        return self._getAPI().getEntityLoader().extractEntity(self, resFile)

    def createNewInternalChild(self, childName):
        if not self.isLoadedToNative():
            raise RuntimeError("Can't create internal child entity '{0}' for entity '{1}' that isn't loaded to edit".format(
                childName, self._name))
        for item in self._children:
            if item.getName() == childName:
                print("[EntityNative:createNewInternalChild] Child with the name '{0}' already exists in entity '{1}'".format(
                    childName, self._name))
                return None
        childId = self._getAPI().getLibrary().createChildEntity(self._entityId, childName)
        if childId == -1:
            print("[EntityNative:createNewInternalChild] Can't add child '{0}' to entity '{1}'".format(
                childName, self._name))
            return None
        childEntId = self._getAPI().getLibrary().getEntityChildEntityId(self._entityId, childId)
        if childEntId == 0:
            raise RuntimeError("[EntityNative:createNewInternalChild] Can't query entity id of child '{0}' of entity '{1}'".format(
                childName, self._name))
        childEntity = EntityNative()
        childEntity._name = childName
        childEntity._entityId = childEntId
        childEntity._parent = self
        childEntity._childId = childId
        childEntity._isInternal = True
        childEntity._isModified = True
        self._getAPI().getEntityLoader().setupDefaultTransform(childEntity)
        childEntity._syncWithNative()
        self._children.append(childEntity)
        self._isModified = True
        return childEntity