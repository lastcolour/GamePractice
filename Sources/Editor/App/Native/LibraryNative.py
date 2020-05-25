from .MemoryStream import MemoryStream

import ctypes
import os
import pathlib

class DrawBuffer:
    def __init__(self):
        self._allocSize = (0, 0)
        self._size = (0, 0)
        self._data = None

    def setSize(self, w, h):
        if self._allocSize[0] >= w and self._allocSize[1] >= h:
            self._size = (w, h)
            return
        self._allocSize = (w, h)
        self._size = self._allocSize
        self._data = ctypes.c_ubyte * self.getDataSize()

    def getDataSize(self):
        return self._size[0] * self._size[1] * 4

    def getCPtr(self):
        return ctypes.cast(self._data(), ctypes.POINTER(ctypes.c_void_p))

class LibraryNative:
    def __init__(self):
        self._drawBuffer = DrawBuffer()

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
        self._getRegisteredEntityLogicsFunc.argstype = [ctypes.POINTER(ctypes.c_void_p),]
        self._getRegisteredEntityLogicsFunc.restype = ctypes.c_uint32
    
        self._loadEntityFunc = self._editorLib.LoadEntity
        self._loadEntityFunc.argstype = [ctypes.c_char_p,]
        self._loadEntityFunc.restype = ctypes.c_uint32

        self._unloadEntityFunc = self._editorLib.UnloadEntity
        self._unloadEntityFunc.argstype = [ctypes.c_uint32,]
        self._unloadEntityFunc.restype = None

        self._getEntityChildrenFunc = self._editorLib.GetEntityChildren
        self._getEntityChildrenFunc.argstype = [ctypes.c_uint32, ctypes.POINTER(ctypes.c_void_p)]
        self._getEntityChildrenFunc.restype = ctypes.c_uint32

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
        self._getEntityLogicDataFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.POINTER(ctypes.c_void_p)]
        self._getEntityLogicDataFunc.restype = ctypes.c_uint32

        self._getEntityLogicValueDataFunc = self._editorLib.GetEntityLogicValueData
        self._getEntityLogicValueDataFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.c_int32, ctypes.POINTER(ctypes.c_void_p)]
        self._getEntityLogicValueDataFunc.restype = ctypes.c_uint32

        self._setEntityLogicDataFunc = self._editorLib.SetEntityLogicData
        self._setEntityLogicDataFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.c_void_p, ctypes.c_uint32]
        self._setEntityLogicDataFunc.restype = None

        self._setEntityLogicValueDataFunc = self._editorLib.SetEntityLogicValueData
        self._setEntityLogicValueDataFunc.argstype = [ctypes.c_uint32, ctypes.c_int32, ctypes.c_int32, ctypes.c_void_p, ctypes.c_uint32]
        self._setEntityLogicValueDataFunc.restype = None

        self._addChildEntityToEntityFunc = self._editorLib.AddChildEntityToEntity
        self._addChildEntityToEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_char_p]
        self._addChildEntityToEntityFunc.restype = ctypes.c_uint32

        self._removeChildEntityFromEntityFunc = self._editorLib.RemoveChildEntityFromEntity
        self._removeChildEntityFromEntityFunc.argstype = [ctypes.c_uint32, ctypes.c_uint32]
        self._removeChildEntityFromEntityFunc.restype = None

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
        cEntityName = ctypes.c_char_p(entityName.encode('ascii'))
        return self._loadEntityFunc(cEntityName)

    def unloadEntity(self, entityId):
        cEntId = ctypes.c_uint32(entityId)
        self._unloadEntityFunc(cEntId)

    def getEntityChildren(self, entityId):
        cEntId = ctypes.c_uint32(entityId)
        outPtr = ctypes.POINTER(ctypes.c_char)()
        childCount = self._getEntityChildrenFunc(cEntId, ctypes.byref(outPtr))
        if childCount == 0:
            return []
        bufferPtr = ctypes.POINTER(ctypes.c_uint32 * childCount)(outPtr.contents)
        res = []
        for i in range(childCount):
            res.append(bufferPtr.contents[i])
        return res

    def getEntityName(self, entityId):
        cEntId = ctypes.c_uint32(entityId)
        res = self._getEntityNameFunc(cEntId)
        return res.decode('ascii')

    def addLogicToEntity(self, entityId, logicName):
        cEntId = ctypes.c_uint32(entityId)
        cLogicName = ctypes.c_char_p(logicName.encode('ascii'))
        return self._addLogicToEntityFunc(cEntId, cLogicName)

    def removeLogicFromEntity(self, entityId, logicId):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        self._removeLogicFromEntityFunc(cEntId, cLogicId)

    def getEntityLogicData(self, entityId, logicId):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        outPtr = ctypes.POINTER(ctypes.c_char)()
        outSize = self._getEntityLogicDataFunc(cEntId, cLogicId, ctypes.byref(outPtr))
        stream = MemoryStream()
        if outSize == 0:
            return stream
        bufferPtr = ctypes.POINTER(ctypes.c_char * outSize)(outPtr.contents)
        stream.setData(bytearray(bufferPtr.contents))
        return stream

    def getEntityLogicValueData(self, entityId, logicId, valueId):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        cValueId = ctypes.c_int32(valueId)
        outPtr = ctypes.POINTER(ctypes.c_char)()
        outSize = self._getEntityLogicValueDataFunc(cEntId, cLogicId, cValueId, ctypes.byref(outPtr))
        stream = MemoryStream()
        if outSize == 0:
            return stream
        bufferPtr = ctypes.POINTER(ctypes.c_char * outSize)(outPtr.contents)
        stream.setData(bytearray(bufferPtr.contents))
        return stream

    def setEntityLogicData(self, entityId, logicId, stream):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        dataPtr = ctypes.c_char * stream.getSize()
        self._setEntityLogicDataFunc(cEntId, cLogicId, dataPtr.from_buffer(stream.getData()), stream.getSize())

    def setEntityLogicValueData(self, entityId, logicId, valueId, stream):
        cEntId = ctypes.c_uint32(entityId)
        cLogicId = ctypes.c_int32(logicId)
        cValueId = ctypes.c_int32(valueId)
        dataPtr = ctypes.c_char * stream.tellg()
        self._setEntityLogicValueDataFunc(cEntId, cLogicId, cValueId, dataPtr.from_buffer(stream.getData()), stream.getSize())

    def drawFrame(self, width, height):
        self._drawBuffer.setSize(width, height)
        self._drawFrameFunc(self._drawBuffer.getCPtr(), ctypes.c_uint32(width), ctypes.c_uint32(height))

    def getRegisteredEntityLogics(self):
        outPtr = ctypes.POINTER(ctypes.c_char_p)()
        outSize = self._getRegisteredEntityLogicsFunc(ctypes.byref(outPtr))
        res = []
        if outSize == 0:
            return res
        outPtr = ctypes.POINTER(ctypes.c_char_p * outSize)(outPtr.contents)
        for i in range(outSize):
            res.append(outPtr.contents[i].decode('ascii'))
        return res

    def addChildEntityToEntity(self, entityId, entityName):
        cEntityName = ctypes.c_char_p(entityName.encode('ascii'))
        cEntId = ctypes.c_uint32(entityId)
        return self._addChildEntityToEntityFunc(cEntId, cEntityName)

    def removeChildEntityFromEntity(self, parentId, childId):
        cParentId = ctypes.c_uint32(parentId)
        cChildId = ctypes.c_uint32(childId)
        self._removeChildEntityFromEntityFunc(cParentId, cChildId)