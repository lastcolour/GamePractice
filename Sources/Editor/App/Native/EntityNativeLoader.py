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
        return "{0}/Entities/{1}".format(self._getAPI().getAssetsRootPath(), filePath)

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
            entity.addLogicWithData(item["type"], item["data"])
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
