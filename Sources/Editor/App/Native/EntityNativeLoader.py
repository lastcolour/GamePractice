from .Native import NativeObject
from .EntityNative import EntityNative
from .LogicNative import LogicNative
from .ValueNative import *
from .LogicNative import CreateLogic

import json

class EntityNativeLoader(NativeObject):
    def __init__(self):
        pass

    def init(self):
        return True

    def _getFullPath(self, filePath):
        return "{0}/Entities/{1}".format(self._getAPI().getAssetsRootDir(), filePath)

    def _loadChildren(self, entity, data):
        if "children" not in data:
            print("[EntityNativeLoader:_loadChildren] Can't find require 'children' node in entity: '{0}'".format(entity._name))
            return False
        childData = data["children"]
        for childName in childData:
            childEntity = self.loadEntity(childName)
            if childEntity is None:
                print("[EntityNativeLoader:_loadChildren] Can't load children: '{0}' for entity: '{1}'".format(
                    childName, entity._name))
                return False
            entity._children.append(childEntity)
        return True

    def _loadLogic(self, data):
        if "type" not in data:
            print("[EntityNativeLoader:_loadLogic] Can't find reuqired 'type' node in logic data")
            return None
        if "data" not in data:
            print("[EntityNativeLoader:_loadLogic] Can't find reuqired 'data' node in logic data")
            return None
        logicType = data["type"]
        logic = CreateLogic(logicType)
        if logic is None:
            return None
        logicData = data["data"]
        logic._rootValue.readFromDict(logicData)
        return logic

    def _loadLogics(self, entity, data):
        if "logics" not in data:
            print("[EntityNativeLoader:_loadLogics] Can't find require 'logics' node in entity: '{0}'".format(entity._name))
            return False
        logicData = data["logics"]
        logicId = 0
        for item in logicData:
            logic = self._loadLogic(item)
            if logic is not None:
                logic._logicId = logicId
                entity._logics.append(logic)
            else:
                print("[EntityNativeLoader:_loadLogics] Can't load logic from entity: '{0}'".format(entity._name))
            logicId += 1
        return True

    def loadEntity(self, filePath):
        fullFilePath = self._getFullPath(filePath)
        with open(fullFilePath) as tFile:
            data = json.load(tFile)
        entity = EntityNative()
        entity._name = filePath
        if not self._loadChildren(entity, data):
            print("[EntityNativeLoader:loadEntity] Can't load children from entity: '{0}'".format(filePath))
            return None
        if not self._loadLogics(entity, data):
            print("[EntityNativeLoader:loadEntity] Can't load logics from entity: '{0}'".format(filePath))
            return None
        return entity
