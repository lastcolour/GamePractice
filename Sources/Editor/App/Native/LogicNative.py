from .Native import NativeObject
from .ValueNative import ObjectValue, AssignValueIdx, CreateObjectValue

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

def CreateLogic(logicType):
    logic = LogicNative()
    logic._name = logicType
    if not CreateObjectValue(logic._rootValue, logicType):
        print("[CreateLogic] Can't create logic values for logic: '{0}'".format(logic._name))
        return None
    AssignValueIdx(logic._rootValue)
    return logic