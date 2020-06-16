from .Native import NativeObject
from .ValueNative import ObjectValue, AssignValueIdx, AssignValueLogic, CreateObjectValue, _syncValueWithNative
from .MemoryStream import MemoryStream

class LogicNative(NativeObject):

    ALL_VALUE_ID = 0

    def __init__(self):
        self._entity = None
        self._logicId = None
        self._name = None
        self._isModified = False
        self._rootValue = ObjectValue()

    def isModified(self):
        return self._isModified

    def getValues(self):
        return self._rootValue._vals

    def getName(self):
        return self._name

    def getEntity(self):
        return self._entity

    def getNativeId(self):
        return self._logicId

    def writeToDict(self, data):
        data["type"] = self._name
        data["id"] = self._logicId
        rootData = []
        self._rootValue.writeToDict(rootData)
        data["data"] = rootData[0]

    def setWriteOnly(self, flag):
        self._rootValue.setWriteOnly(flag)

    def writeToStream(self, stream):
        self._rootValue.writeToStream(stream)

    def readFromNative(self):
        if self._rootValue._isWriteOnly:
            return
        stream = self._getAPI().getLibrary().getEntityLogicData(self._entity.getNativeId(), self.getNativeId(), LogicNative.ALL_VALUE_ID)
        self._rootValue.readFromStream(stream)

def CreateLogic(logicType):
    logic = LogicNative()
    logic._name = logicType
    if not CreateObjectValue(logic._rootValue, logicType):
        print("[CreateLogic] Can't create logic values for logic: '{0}'".format(logic._name))
        return None
    AssignValueIdx(logic._rootValue)
    AssignValueLogic(logic._rootValue, logic)
    return logic