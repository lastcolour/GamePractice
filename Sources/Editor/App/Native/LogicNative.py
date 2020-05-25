from .Native import NativeObject
from .ValueNative import ObjectValue, AssignValueIdx, AssignValueLogic, CreateObjectValue
from .MemoryStream import MemoryStream

class LogicNative(NativeObject):
    def __init__(self):
        self._entity = None
        self._logicId = None
        self._name = None
        self._rootValue = ObjectValue()

    def getValues(self):
        return self._rootValue._vals

    def getName(self):
        return self._name

    def getEntity(self):
        return self._entity

    def getNativeId(self):
        return self._logicId

    def writeToStream(self, stream):
        self._rootValue.writeToStream(stream)

    def readFromNative(self):
        stream = self._getAPI().getLibrary().getEntityLogicData(self._entity.getNativeId(), self.getNativeId())
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