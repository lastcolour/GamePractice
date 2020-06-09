from .Native import NativeObject
from .EntityNative import EntityNative
from .LogicNative import LogicNative
from .ValueNative import *
from .LogicNative import CreateLogic

import json
import os

_DEFAULT_TRANSFORM = {
    "pos":{"x":0, "y":0, "z":0},
    "rot":{"x":0, "y":0, "z":0, "w":1},
    "scale":{"x":1, "y":1, "z":1}
}
class EntityNativeLoader(NativeObject):
    def __init__(self):
        self._entitiesStack = []

    def init(self):
        return True

    def getEntityFullPath(self, filePath):
        return "{0}/Entities/{1}".format(self._getAPI().getAssetsRootPath(), filePath)

    def _loadChildren(self, entity, data):
        if "children" not in data:
            print("[EntityNativeLoader:_loadChildren] Can't find required 'children' node in entity: '{0}'".format(entity._name))
            return False
        childData = data["children"]
        for childNode in childData:
            if "name" not in childNode:
                print("[EntityNativeLoader:_loadChildren] Can't find required 'name' field in children section in entity: '{0}'".format(entity._name))
                return False
            if "id" not in childNode:
                print("[EntityNativeLoader:_loadChildren] Can't find required 'id' field in children section in entity: '{0}'".format(entity._name))
                return False
            if "transform" not in childNode:
                print("[EntityNativeLoader:_loadChildren] Can't find required 'transform' field in children section in entity: '{0}'".format(entity._name))
                return False
            childName = childNode["name"]
            childId = childNode["id"]
            childTm = childNode["transform"]
            childEntity = self.loadEntity(childName)
            if not childEntity.initTransformLogic(childTm):
                print("[EntityNativeLoader:_loadChildren] Can't load transfrom for child entity: '{0}'".format(childName))
                return False
            if childEntity is None:
                print("[EntityNativeLoader:_loadChildren] Can't load children: '{0}' for entity: '{1}'".format(
                    childName, entity._name))
                return False
            childEntity._childId = childId
            entity._children.append(childEntity)
        return True

    def _loadLogics(self, entity, data):
        if "logics" not in data:
            print("[EntityNativeLoader:_loadLogics] Can't find require 'logics' node in entity: '{0}'".format(entity._name))
            return False
        logicData = data["logics"]
        for item in logicData:
            if "type" not in item:
                print("[EntityNativeLoader:_loadLogics] Can't find reuqired 'type' node in logic data")
                return None
            if "data" not in item:
                print("[EntityNativeLoader:_loadLogics] Can't find reuqired 'data' node in logic data")
                return None
            if "id" not in item:
                print("[EntityNativeLoader:_loadLogics] Can't find reuqired 'id' node in logic data")
                return None
            entity.addLogicWithData(item["type"], item["id"], item["data"])
        return True

    def loadEntity(self, entityName):
        if entityName in self._entitiesStack:
            print("[EntityNativeLoader:loadEntity] Found cyclic dependecy: [{0}]".format(", ".join(self._entitiesStack)))
            self._entitiesStack.clear()
            raise RuntimeError("Can't load entity: {0}".format(entityName))
        self._entitiesStack.append(entityName)
        try:
            resEntity = self._loadEntityImpl(entityName)
            resEntity.initTransformLogic(_DEFAULT_TRANSFORM)
        except:
            self._entitiesStack.pop()
            raise
        self._entitiesStack.pop()
        return resEntity

    def _loadEntityImpl(self, entityName):
        fullFilePath = self.getEntityFullPath(entityName)
        if not os.path.exists(fullFilePath):
            print("[EntityNativeLoader:loadEntity] Can't load entity '{0}' from missed file".format(fullFilePath))
            return None
        with open(fullFilePath) as tFile:
            data = json.load(tFile)
        entity = EntityNative()
        entity._name = entityName
        if not self._loadLogics(entity, data):
            print("[EntityNativeLoader:loadEntity] Can't load logics from entity: '{0}'".format(entityName))
            return None
        if not self._loadChildren(entity, data):
            print("[EntityNativeLoader:loadEntity] Can't load children from entity: '{0}'".format(entityName))
            return None
        return entity

    def saveEntity(self, entity):
        fullFilePath = self.getEntityFullPath(entity.getName())
        data = entity.dumpToDict()
        with open(fullFilePath, 'w') as tFile:
            json.dump(data, tFile, indent=2)
        print("[EntityNativeLoader:saveEntity] Save entity changes: '{0}'".format(entity.getName()))

def CreateVoidEntity(filePath):
    data = {
        "children":[],
        "logics":[]
    }
    with open(filePath, 'w+') as tFile:
        json.dump(data, tFile, indent=2)
    return True