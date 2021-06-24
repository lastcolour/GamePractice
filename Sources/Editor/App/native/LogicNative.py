from .Native import NativeObject
from .ValueNative import ObjectValue, AssignValueIdx, _createValue
from .MemoryStream import MemoryStream

import traceback

class LogicNative(NativeObject):

    ALL_VALUE_ID = 0

    def __init__(self):
        self._entity = None
        self._logicId = None
        self._name = None
        self._isModified = False
        self._isEditorDataDiffer = False
        self._autoWriteToNative = True
        self._rootValue = None

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

    def setAutoWriteToNative(self, flag):
        self._autoWriteToNative = flag

    def writeToDict(self, data):
        data["type"] = self._name
        data["id"] = self._logicId
        rootData = []
        self._rootValue.writeToDict(rootData)
        data["data"] = rootData[0]

    def readFromCopyDict(self, data):
        self._rootValue.readFromDict(data)
        self.writeToNative()
        self.readFromNative()

    def writeToStream(self, stream):
        self._rootValue.writeToStream(stream)

    def isEditorDataDiffer(self):
        return self._isEditorDataDiffer

    def _onValueChanged(self):
        self._isEditorDataDiffer = True
        if self._autoWriteToNative:
            self.writeToNative()

    def readFromNative(self):
        print("[LogicNative:readFromNative] Read '{0}'".format(self._name))
        try:
            stream = self._getAPI().getLibrary().getEntityLogicData(self._entity.getNativeId(), self._logicId, LogicNative.ALL_VALUE_ID)
            self._rootValue.readFromStream(stream)
        except Exception as e:
            print("[LogicNative:readFromNative] Error occured during deserializtion from native data: '{0}' (Data: {1})".format(
                self._name, stream._data))
            traceback.print_tb(e.__traceback__)

    def writeToNative(self):
        print("[LogicNative:writeToNative] Write '{0}'".format(self._name))
        self._isEditorDataDiffer = False
        stream = MemoryStream()
        self.writeToStream(stream)
        self._getAPI().getLibrary().setEntityLogicData(self._entity.getNativeId(), self._logicId, LogicNative.ALL_VALUE_ID, stream)

def CreateLogic(logicType):
    logic = LogicNative()
    logic._name = logicType
    logic._rootValue = _createValue(None, logic, logicType)
    AssignValueIdx(logic._rootValue)
    return logic