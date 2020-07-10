from .MemoryStream import MemoryStream

import ctypes
import os
import pathlib

class LibraryNative:
    def __init__(self):
        pass

    def initialize(self, libPath):
        os.chdir(pathlib.Path(libPath).parent.__str__())
        self._editorLib = ctypes.CDLL(libPath)

        self._initializeFunc = self._editorLib.Initiliaze
        self._initializeFunc.argstype = None
        self._initializeFunc.restype = ctypes.c_uint32

        self._deinitializeFunc = self._editorLib.DeInitialize
        self._deinitializeFunc.argstype = None
        self._deinitializeFunc.restype = None

        self._getReflectModelFunc = self._editorLib.GetReflectModel
        self._getReflectModelFunc.argstype = None
        self._getReflectModelFunc.restype = ctypes.c_char_p

        self._getRegisteredEntityLogicsFunc = self._editorLib.GetRegisteredEntityLogics
        self._getRegisteredEntityLogicsFunc.argstype = None
        self._getRegisteredEntityLogicsFunc.restype = ctypes.c_char_p

        self._loadEntityFunc = self._editorLib.LoadEntity
        self._loadEntityFunc.argstype = [ctypes.c_char_p,]
        self._loadEntityFunc.restype = ctypes.c_uint32

        self._unloadEntityFunc = self._editorLib.UnloadEntity
        self._unloadEntityFunc.argstype = [ctypes.c_uint32,]
        self._unloadEntityFunc.restype = None

        self._getEntityChildEntityIdFunc = self._editorLib.GetEntityChildEntityId
        self._getEntityChildEntityIdFunc.argstype = [ctypes.c_uint32, ctypes.c_uint32]
        self._getEntityChildEntityIdFunc.restype = ctypes.c_int32

        self._getEntityNameFunc = self._editorLib.GetEntityName
        self._getEntityNameFunc.argstype = [ctypes.c_uint32,]
        self._getEntityNameFunc.restype = ctypes.c_char_p

        self._addLogicToEntityFunc = self._editorLib.AddLogicToEntity
        self._addLogicToEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_char_p]
        self._addLogicToEntityFunc.restype = ctypes.c_int32

        self._removeLogicFromEntityFunc = self._editorLib.RemoveLogicFromEntity
        self._removeLogicFromEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_int32]
        self._removeLogicFromEntityFunc.restype = None

        self._getEntityLogicDataFunc = self._editorLib.GetEntityLogicData
        self._getEntityLogicDataFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.c_int32, ctypes.POINTER(ctypes.c_void_p)]
        self._getEntityLogicDataFunc.restype = ctypes.c_uint32

        self._setEntityLogicDataFunc = self._editorLib.SetEntityLogicData
        self._setEntityLogicDataFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.c_int32, ctypes.c_void_p, ctypes.c_uint32]
        self._setEntityLogicDataFunc.restype = None

        self._addEntityLogicArrayElementFunc = self._editorLib.AddEntityLogicArrayElement
        self._addEntityLogicArrayElementFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.c_int32]
        self._addEntityLogicArrayElementFunc.restype = None

        self._addChildEntityToEntityFunc = self._editorLib.AddChildEntityToEntity
        self._addChildEntityToEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_uint32]
        self._addChildEntityToEntityFunc.restype = ctypes.c_int32

        self._removeChildEntityFromEntityFunc = self._editorLib.RemoveChildEntityFromEntity
        self._removeChildEntityFromEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_uint32]
        self._removeChildEntityFromEntityFunc.restype = None

        self._createChildEntityFunc = self._editorLib.CreateChildEntity
        self._createChildEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_char_p]
        self._createChildEntityFunc.restype = ctypes.c_int32

        self._renameEntityFunc = self._editorLib.RenameEntity
        self._renameEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_char_p]
        self._renameEntityFunc.restype = ctypes.c_int32

        self._mouseInputEventFunc = self._editorLib.MouseInputEvent
        self._mouseInputEventFunc.argstype = [ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint32]
        self._mouseInputEventFunc.restype = None

        self._setGameTimeScaleFunc = self._editorLib.SetGameTimeScale
        self._setGameTimeScaleFunc.argstype = [ctypes.c_float]
        self._setGameTimeScaleFunc.restype = None

        self._enableGameUpdateFunc = self._editorLib.EnableGameUpdate
        self._enableGameUpdateFunc.argstype = [ctypes.c_bool]
        self._enableGameUpdateFunc.restype = None

        self._unloadAllFunc = self._editorLib.UnloadAll
        self._unloadAllFunc.argstype = None
        self._unloadAllFunc.restype = None

        self._drawFrameFunc = self._editorLib.DrawFrame
        self._drawFrameFunc.argstype = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
        self._drawFrameFunc.restype = None

        return self._initializeFunc()

    def deinitialize(self):
        self._deinitializeFunc()

    def getReflectModel(self):
        model = self._getReflectModelFunc()
        return model.decode('ascii')

    def loadEntity(self, entityName):
        cEntityName = ctypes.c_char_p(entityName.encode('ascii') + b'\x00')
        return self._loadEntityFunc(cEntityName)

    def unloadEntity(self, entityId):
        cEntId = ctypes.c_uint32(entityId)
        self._unloadEntityFunc(cEntId)

    def getEntityChildEntityId(self, parentId, childId):
        cParentId = ctypes.c_uint32(parentId)
        cChildId = ctypes.c_uint32(childId)
        return self._getEntityChildEntityIdFunc(cParentId, cChildId)

    def getEntityName(self, entityId):
        cEntId = ctypes.c_uint32(entityId)
        res = self._getEntityNameFunc(cEntId)
        return res.decode('ascii')

    def addLogicToEntity(self, entityId, logicName):
        cEntId = ctypes.c_uint32(entityId)
        cLogicName = ctypes.c_char_p(logicName.encode('ascii') + b'\x00')
        return self._addLogicToEntityFunc(cEntId, cLogicName)

    def removeLogicFromEntity(self, entityId, logicId):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        self._removeLogicFromEntityFunc(cEntId, cLogicId)

    def getEntityLogicData(self, entityId, logicId, valueId):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        cValueId = ctypes.c_int32(valueId)
        outPtr = ctypes.POINTER(ctypes.c_char)()
        outSize = self._getEntityLogicDataFunc(cEntId, cLogicId, cValueId, ctypes.byref(outPtr))
        stream = MemoryStream()
        if outSize == 0:
            return stream
        bufferPtr = ctypes.POINTER(ctypes.c_char * outSize)(outPtr.contents)
        stream.setData(bytearray(bufferPtr.contents))
        return stream

    def setEntityLogicData(self, entityId, logicId, valueId, stream):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        cValueId = ctypes.c_int32(valueId)
        dataPtr = ctypes.c_char * stream.tellg()
        self._setEntityLogicDataFunc(cEntId, cLogicId, cValueId, dataPtr.from_buffer(stream.getData()), stream.getSize())

    def drawFrame(self, ptr, width, height):
        ptr = ctypes.c_void_p(ptr)
        self._drawFrameFunc(ptr, ctypes.c_uint32(width), ctypes.c_uint32(height))

    def getRegisteredEntityLogics(self):
        res = self._getRegisteredEntityLogicsFunc()
        return res.decode('ascii')

    def addChildEntityToEntity(self, parentId, childId):
        cParentId = ctypes.c_uint32(parentId)
        cChildId = ctypes.c_uint32(childId)
        return self._addChildEntityToEntityFunc(cParentId, cChildId)

    def removeChildEntityFromEntity(self, parentId, childId):
        cParentId = ctypes.c_uint32(parentId)
        cChildId = ctypes.c_uint32(childId)
        self._removeChildEntityFromEntityFunc(cParentId, cChildId)

    def addEntityLogicArrayElement(self, entityId, logicId, valueId):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        cValueId = ctypes.c_int32(valueId)
        self._addEntityLogicArrayElementFunc(cEntId, cLogicId, cValueId)

    def createChildEntity(self, entityId, childName):
        cEntId = ctypes.c_uint32(entityId)
        cChildName = ctypes.c_char_p(childName.encode('ascii') + b'\x00')
        return self._createChildEntityFunc(cEntId, cChildName)

    def mouseInputEvent(self, actionType, xPos, yPos):
        cType = ctypes.c_uint32(actionType)
        cXPos = ctypes.c_uint32(xPos)
        cYPos = ctypes.c_uint32(yPos)
        self._mouseInputEventFunc(cType, cXPos, cYPos)

    def unloadAll(self):
        self._unloadAllFunc()

    def setGameTimeScale(self, scale):
        cScale = ctypes.c_float(scale)
        self._setGameTimeScaleFunc(cScale)

    def enableGameUpdate(self, flag):
        cFlag = ctypes.c_bool(flag)
        self._enableGameUpdateFunc(cFlag)

    def renameEntity(self, entityId, newName):
        cEntId = ctypes.c_uint32(entityId)
        cName = ctypes.c_char_p(newName.encode('ascii') + b'\x00')
        res = self._renameEntityFunc(cEntId, cName)
        return res == 0