from .Native import NativeObject

import json
import collections

class ReflectModel(NativeObject):
    def __init__(self):
        self._model = {}
        self._logics = {}
        self._inheritanceTree = {}

    def init(self):
        nativeLib = self._getAPI().getLibrary()
        cModel = nativeLib.getReflectModel()
        self._model = json.loads(cModel, object_pairs_hook=collections.OrderedDict)
        self._createInheritanceTree()
        logics = nativeLib.getRegisteredEntityLogics()
        self._logics = json.loads(logics)
        return True

    def getEnumTable(self, enumType):
        if enumType not in self._model:
            return None
        return self._model[enumType]["data"]

    def getAllRegisteredLogics(self):
        return self._logics

    def _createInheritanceTree(self):
        for item in self._model:
            classInfo = self._model[item]
            if classInfo["type"] != "class" and classInfo["type"] != "abs_class":
                continue
            if "base" not in classInfo:
                continue
            for baseClass in classInfo["base"]:
                if baseClass not in self._inheritanceTree:
                    self._inheritanceTree[baseClass] = []
                self._inheritanceTree[baseClass].append(item)

    def _buildClassData(self, classModel, classResData):
        if "base" in classModel:
            for baseType in classModel["base"]:
                baseModel = self.getTypeModel(baseType)
                if baseModel is None:
                    raise RuntimeError("Can't find type model for a base class: '{0}'".format(baseType))
                if "data" not in baseModel:
                    continue
                baseModelData = baseModel["data"]
                for item in baseModelData:
                    classResData[item] = baseModelData[item]
        if "data" in classModel:
            classData = classModel["data"]
            for item in classData:
                classResData[item] = classData[item]

    def getLogicModel(self, logicType):
        logicExists = None
        for module in self._logics:
            if logicType in self._logics[module]:
                logicExists = True
        if not logicExists:
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

    def getAllDerivedTypes(self, baseType):
        res = ["Null",]
        q = [baseType,]
        while q:
            typeName = q.pop()
            if self._model[typeName]["type"] == "class":
                res.append(typeName)
            if typeName in self._inheritanceTree:
                for item in self._inheritanceTree[typeName]:
                    q.append(item)
        if len(q) == 1:
            print("[ReflectModel:getAllDerivedTypes] Can't find valid deriveded types for a base type '{0}' in model".format(
                baseType))
        return res