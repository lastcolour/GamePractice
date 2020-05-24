from .Native import NativeObject

import json
import collections

class ReflectModel(NativeObject):
    def __init__(self):
        self._model = {}
        self._logics = []

    def init(self):
        nativeLib = self._getAPI().getLibrary()
        cModel = nativeLib.getReflectModel()
        self._model = json.loads(cModel, object_pairs_hook=collections.OrderedDict)
        self._logics = nativeLib.getRegisteredEntityLogics()
        return True

    def getEnumTable(self, enumType):
        if enumType not in self._model:
            return None
        return self._model[enumType]["data"]

    def getArrayElemType(self, arrayType):
        if arrayType not in self._model:
            return None
        return self._model[arrayType]["data"]["type"]

    def getAllRegisteredLogic(self):
        return self._logics

    def _buildClassData(self, classModel, classResData):
        if "base" in classModel:
            for baseType in classModel["base"]:
                baseModel = self.getTypeModel(baseType)
                if baseModel is None:
                    raise RuntimeError("Can't find type model for a base class: '{0}'".format(baseType))
                baseModelData = baseModel["data"]
                for item in baseModelData:
                    classResData[item] = baseModelData[item]
        if "data" in classModel:
            classData = classModel["data"]
            for item in classData:
                classResData[item] = classData[item]

    def getLogicModel(self, logicType):
        if logicType not in self._logics:
            print("[ReflectModel:getLogicModel] Can't find logic type '{0}' in registered logics".format(
                logicType))
            return None
        return self.getTypeModel(logicType)

    def getTypeModel(self, typeName):
        if typeName not in self._model:
            print("[ReflectModel:getTypeModel] Can't find type '{0}' in model".format(
                typeName))
            return None
        typeModel = self._model[typeName]
        if typeModel["type"] != "class":
            return typeModel
        if "base" not in typeModel:
            return typeModel
        resModel = {"type":"class", "data": collections.OrderedDict()}
        self._buildClassData(typeModel, resModel["data"])
        return resModel