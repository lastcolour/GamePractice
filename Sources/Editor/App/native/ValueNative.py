from .Native import NativeObject
from .MemoryStream import MemoryStream

import collections

def _getReflectModel():
    return NativeObject._NATIVE_API.getReflectModel()

def _syncValueWithNative(val):
    stream = MemoryStream()
    val.writeToStream(stream)
    NativeObject._NATIVE_API.getLibrary().setEntityLogicData(val.getEntityId(), val.getLogicId(), val._valueId, stream)
    stream = NativeObject._NATIVE_API.getLibrary().getEntityLogicData(val.getEntityId(), val.getLogicId(), val._valueId)
    val.readFromStream(stream)

class ValueType:
    Bool = 0
    Int = 1
    Float = 2
    String = 3
    Vec2i = 4
    Vec2 = 5
    Vec3 = 6
    Vec4 = 7
    Quat = 9
    Color = 10
    Enum = 11
    Resource = 12
    Entity = 13
    Object = 14
    Array = 15

class ResourceType:
    Invalid = 0
    Entity = 1
    Image = 2
    Sound = 3
    SoundEvent = 4

    @staticmethod
    def getResourceType(valType):
        if valType == "Entity":
            return ResourceType.Entity
        elif valType == "Image":
            return ResourceType.Image
        elif valType == "Sound":
            return ResourceType.Sound
        elif valType == "SoundEvent":
            return ResourceType.SoundEvent
        else:
            raise RuntimeError("Invalid resource type '{0}'".format(valType))

class ValueNative(NativeObject):
    def __init__(self, valueType):
        self._name = None
        self._arrayVal = None
        self._isArrayElement = False
        self._logic = None
        self._valueId = None
        self._isModified = False
        self._type = valueType

    def getName(self):
        return self._name

    def getType(self):
        return self._type

    def getPrimitiveValueCount(self):
        return 1

    def isModified(self):
        return self._isModified

    def getEntity(self):
        if self._logic is not None:
            return self._logic.getEntity()
        if self._arrayVal is not None:
            return self._arrayVal.getEntity()
        return None

    def getLogicId(self):
        if self._logic is not None:
            return self._logic.getNativeId()
        if self._arrayVal is not None:
            return self._arrayVal.getLogicId()
        return None

    def getEntityId(self):
        ent = self.getEntity()
        if ent is None:
            return None
        return ent.getNativeId()

    def _isLoadedToNative(self):
        ent = self.getEntity()
        if ent is None:
            return None
        return ent.isLoadedToNative()

    def _setArrayVal(self, arrayVal):
        if self._logic is not None:
            raise RuntimeError("Value already belongs to a logic")
        self._arrayVal = arrayVal

    def _onValueChanged(self):
        if self.getPrimitiveValueCount() != 1:
            raise RuntimeError("Can't write to native non-primitive values")
        if not self._isLoadedToNative():
            return
        if self._arrayVal is None:
            _syncValueWithNative(self)
        else:
            _syncValueWithNative(self._arrayVal)
        self._isModified = True

class BoolValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Bool)
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._val = bool(node[self._name])
        else:
            self._val = bool(node)

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readBool()

    def writeToStream(self, stream):
        stream.writeBool(self._val)

    def setVal(self, flag):
        self._val = bool(flag)
        self._onValueChanged()

    def getVal(self):
        return self._val

class IntValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Int)
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._val = int(node[self._name])
        else:
            self._val = int(node)

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readInt()

    def writeToStream(self, stream):
        stream.writeInt(self._val)

    def setVal(self, val):
        self._val = int(val)
        self._onValueChanged()

    def getVal(self):
        return self._val

class FloatValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Float)
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._val = float(node[self._name])
        else:
            self._val = float(node)

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readFloat()

    def writeToStream(self, stream):
        stream.writeFloat(self._val)

    def setVal(self, val):
        self._val = float(val)
        self._onValueChanged()

    def getVal(self):
        return self._val

class StringValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.String)
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._val = str(node[self._name])
        else:
            self._val = str(node)

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readString()

    def writeToStream(self, stream):
        stream.writeString(self._val)

    def setVal(self, val):
        self._val = str(val)
        self._onValueChanged()

    def getVal(self):
        return self._val

class Vec2iValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec2i)
        self._xVal = None
        self._yVal = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._xVal = int(node[self._name]["x"])
            self._yVal = int(node[self._name]["y"])
        else:
            self._xVal = int(node["x"])
            self._yVal = int(node["y"])

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = {"x": self._xVal, "y": self._yVal}
        else:
            node.append({"x": self._xVal, "y": self._yVal})

    def readFromStream(self, stream):
        self._xVal = stream.readInt()
        self._yVal = stream.readInt()

    def writeToStream(self, stream):
        stream.writeInt(self._xVal)
        stream.writeInt(self._yVal)

    def setVal(self, xVal, yVal):
        self._xVal = int(xVal)
        self._yVal = int(yVal)
        self._onValueChanged()

    def getVal(self):
        return self._xVal, self._yVal

class Vec2Value(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec2)
        self._xVal = None
        self._yVal = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._xVal = float(node[self._name]["x"])
            self._yVal = float(node[self._name]["y"])
        else:
            self._xVal = float(node["x"])
            self._yVal = float(node["y"])

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = {"x": self._xVal, "y": self._yVal}
        else:
            node.append({"x": self._xVal, "y": self._yVal})

    def readFromStream(self, stream):
        self._xVal = stream.readFloat()
        self._yVal = stream.readFloat()

    def writeToStream(self, stream):
        stream.writeFloat(self._xVal)
        stream.writeFloat(self._yVal)

    def setVal(self, xVal, yVal):
        self._xVal = float(xVal)
        self._yVal = float(yVal)
        self._onValueChanged()

    def getVal(self):
        return self._xVal, self._yVal

class Vec3Value(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec3)
        self._xVal = None
        self._yVal = None
        self._zVal = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._xVal = float(node[self._name]["x"])
            self._yVal = float(node[self._name]["y"])
            self._zVal = float(node[self._name]["z"])
        else:
            self._xVal = float(node["x"])
            self._yVal = float(node["y"])
            self._zVal = float(node["z"])

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = {"x": self._xVal, "y": self._yVal, "z": self._zVal}
        else:
            node.append({"x": self._xVal, "y": self._yVal, "z": self._zVal})

    def readFromStream(self, stream):
        self._xVal = stream.readFloat()
        self._yVal = stream.readFloat()
        self._zVal = stream.readFloat()

    def writeToStream(self, stream):
        stream.writeFloat(self._xVal)
        stream.writeFloat(self._yVal)
        stream.writeFloat(self._zVal)

    def setVal(self, xVal, yVal, zVal):
        self._xVal = float(xVal)
        self._yVal = float(yVal)
        self._zVal = float(zVal)
        self._onValueChanged()

    def getVal(self):
        return self._xVal, self._yVal, self._zVal

class Vec4Value(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec4)
        self._xVal = None
        self._yVal = None
        self._zVal = None
        self._wVal = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._xVal = float(node[self._name]["x"])
            self._yVal = float(node[self._name]["y"])
            self._zVal = float(node[self._name]["z"])
            self._wVal = float(node[self._name]["w"])
        else:
            self._xVal = float(node["x"])
            self._yVal = float(node["y"])
            self._zVal = float(node["z"])
            self._wVal = float(node["w"])

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = {"x": self._xVal, "y": self._yVal, "z": self._zVal, "w": self._wVal}
        else:
            node.append({"x": self._xVal, "y": self._yVal, "z": self._zVal, "w": self._wVal})

    def readFromStream(self, stream):
        self._xVal = stream.readFloat()
        self._yVal = stream.readFloat()
        self._zVal = stream.readFloat()
        self._wVal = stream.readFloat()

    def writeToStream(self, stream):
        stream.writeFloat(self._xVal)
        stream.writeFloat(self._yVal)
        stream.writeFloat(self._zVal)
        stream.writeFloat(self._wVal)

    def setVal(self, xVal, yVal, zVal, wVal):
        self._xVal = float(xVal)
        self._yVal = float(yVal)
        self._zVal = float(zVal)
        self._wVal = float(wVal)
        self._onValueChanged()

    def getVal(self):
        return self._xVal, self._yVal, self._zVal, self._wVal

class QuatValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Quat)
        self._xVal = None
        self._yVal = None
        self._zVal = None
        self._wVal = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._xVal = float(node[self._name]["x"])
            self._yVal = float(node[self._name]["y"])
            self._zVal = float(node[self._name]["z"])
            self._wVal = float(node[self._name]["w"])
        else:
            self._xVal = float(node["x"])
            self._yVal = float(node["y"])
            self._zVal = float(node["z"])
            self._wVal = float(node["w"])

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = {"x": self._xVal, "y": self._yVal, "z": self._zVal, "w": self._wVal}
        else:
            node.append({"x": self._xVal, "y": self._yVal, "z": self._zVal, "w": self._wVal})

    def readFromStream(self, stream):
        self._xVal = stream.readFloat()
        self._yVal = stream.readFloat()
        self._zVal = stream.readFloat()
        self._wVal = stream.readFloat()

    def writeToStream(self, stream):
        stream.writeFloat(self._xVal)
        stream.writeFloat(self._yVal)
        stream.writeFloat(self._zVal)
        stream.writeFloat(self._wVal)

    def setVal(self, xVal, yVal, zVal, wVal):
        self._xVal = float(xVal)
        self._yVal = float(yVal)
        self._zVal = float(zVal)
        self._wVal = float(wVal)
        self._onValueChanged()

    def getVal(self):
        return self._xVal, self._yVal, self._zVal, self._wVal

class ColorValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Color)
        self._rVal = None
        self._gVal = None
        self._bVal = None
        self._aVal = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._rVal = int(node[self._name]["r"])
            self._gVal = int(node[self._name]["g"])
            self._bVal = int(node[self._name]["b"])
            self._aVal = int(node[self._name]["a"])
        else:
            self._rVal = int(node["r"])
            self._gVal = int(node["g"])
            self._bVal = int(node["b"])
            self._aVal = int(node["a"])

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = {"r": self._rVal, "g": self._gVal, "b": self._bVal, "a": self._aVal}
        else:
            node.append({"r": self._rVal, "g": self._gVal, "b": self._bVal, "a": self._aVal})

    def readFromStream(self, stream):
        self._rVal = stream.readUChar()
        self._gVal = stream.readUChar()
        self._bVal = stream.readUChar()
        self._aVal = stream.readUChar()

    def writeToStream(self, stream):
        stream.writeUChar(self._rVal)
        stream.writeUChar(self._gVal)
        stream.writeUChar(self._bVal)
        stream.writeUChar(self._aVal)

    def setVal(self, rVal, gVal, bVal, aVal):
        self._rVal = int(rVal)
        self._gVal = int(gVal)
        self._bVal = int(bVal)
        self._aVal = int(aVal)
        self._onValueChanged()

    def getVal(self):
        return self._rVal, self._gVal, self._bVal, self._aVal

class ArrayValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Array)
        self._vals = None
        self._elemType = None

    def readFromDict(self, node):
        if self._name is None:
            raise RuntimeError("Array of arrays is not supported")
        self._vals = []
        idx = 0
        for elemNode in node[self._name]:
            elem = _createValue("[{0}]".format(idx), self._elemType)
            idx += 1
            elem._setArrayVal(self)
            elem._isArrayElement = True
            elem.readFromDict(elemNode)
            self._vals.append(elem)

    def writeToDict(self, node):
        if self._name is None:
            raise RuntimeError("Array of arrays is not supported")
        resList = []
        for item in self._vals:
            item.writeToDict(resList)
        node[self._name] = resList

    def readFromStream(self, stream):
        size = stream.readInt()
        if self._vals is None:
            self._vals = []
        newElemsCount = size - len(self._vals)
        if newElemsCount > 1:
            raise RuntimeError("Array size was changed in an invalid way")
        idx = 0
        self._val = self._vals[:size]
        for item in self._vals:
            item.readFromStream(stream)
            item._name = "[{0}]".format(idx)
            idx += 1
        for i in range(newElemsCount):
            elem = _createValue("[{0}]".format(idx), self._elemType)
            idx += 1
            elem._setArrayVal(self)
            elem._isArrayElement = True
            elem.readFromStream(stream)
            self._vals.append(elem)

    def writeToStream(self, stream):
        stream.writeInt(len(self._vals))
        for elem in self._vals:
            elem.writeToStream(stream)

    def addNewElement(self):
        currSize = len(self._vals)
        self._getAPI().getLibrary().addEntityLogicArrayElement(self.getEntityId(), self.getLogicId(), self._valueId)
        stream = self._getAPI().getLibrary().getEntityLogicData(self.getEntityId(), self.getLogicId(), self._valueId)
        self.readFromStream(stream)
        if len(self._vals) != currSize + 1:
            raise RuntimeError("Native error when adding ne element")
        return self._vals[-1]

    def removeElement(self, arrayVal):
        if arrayVal not in self._vals:
            raise RuntimeError("Can't find an value in array elements")
        self._vals.remove(arrayVal)
        self._onValueChanged()

    def getValues(self):
        return self._vals

class EnumValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Enum)
        self._table = {}
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            strVal = str(node[self._name])
        else:
            strVal = str(node)
        self._val = int(self._table[strVal])

    def writeToDict(self, node):
        for item in self._table:
            if self._table[item] == self._val:
                if not self._isArrayElement:
                    node[self._name] = str(item)
                else:
                    node.append(str(item))
                return
        raise RuntimeError("Can't find enum value in table")

    def readFromStream(self, stream):
        self._val = stream.readInt()

    def writeToStream(self, stream):
        stream.writeInt(self._val)

    def setVal(self, val):
        self._val = int(self._table[str(val)])
        self._onValueChanged()

    def getVal(self):
        for item in self._table:
            if self._table[item] == self._val:
                return item
        raise RuntimeError("Can't find enum value in table")

    def getTable(self):
        return self._table

class ObjectValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Object)
        self._vals = []

    def getValues(self):
        return self._vals

    def readFromDict(self, node):
        objectNode = node
        if not self._isArrayElement and self._name is not None:
            objectNode = node[self._name]
        for val in self._vals:
            val.readFromDict(objectNode)

    def writeToDict(self, node):
        objectNode = {}
        for val in self._vals:
            val.writeToDict(objectNode)
        if not self._isArrayElement and self._name is not None:
            node[self._name] = objectNode
        else:
            node.append(objectNode)

    def readFromStream(self, stream):
        for item in self._vals:
            try:
                item.readFromStream(stream)
            except:
                print("[ObjectValue:readFromStream] Can't read value: '{0}'".format(item._name))
                raise

    def writeToStream(self, stream):
        for item in self._vals:
            item.writeToStream(stream)

    def getPrimitiveValueCount(self):
        total = 0
        for item in self._vals:
            total += self._vals[item].getPrimitiveValueCount()
        return total

    def _setArrayVal(self, arrayVal):
        self._arrayVal = arrayVal
        for item in self._vals:
            item._setArrayVal(arrayVal)

class ResourceValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Resource)
        self._resType = None
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._val = str(node[self._name])
        else:
            self._val = str(node)

    def writeToDict(self, node):
        if self._val is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        stream.readString()
        if self._val is None:
            self._val = ""

    def writeToStream(self, stream):
        stream.writeString(self._val)

    def getResourceType(self):
        return self._resType

    def setVal(self, val):
        self._val = str(val)
        self._onValueChanged()

    def getVal(self):
        return self._val

class EntityValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Entity)
        self._val = None

    def readFromDict(self, node):
        if not self._isArrayElement:
            self._val = str(node[self._name])
        else:
            self._val = str(node)

    def writeToDict(self, node):
        if not self._isArrayElement:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readInt()

    def writeToStream(self, stream):
        if isinstance(self._val, str):
            stream.writeInt(-1)
        else:
            stream.writeInt(self._val)

    def setEntityValue(self, entity):
        if entity is None:
            self._val = -1
        else:
            if self.getEntityId() == entity.getNativeId():
                self._val = 0
            else:
                self._val = entity._childId
        self._onValueChanged()

    def getEntityValue(self):
        return self.getEntity().getChildWithId(self._val)

def _createValue(valueName, valueType):
    val = None
    if valueType == "bool":
        val = BoolValue()
    elif valueType == "int":
        val = IntValue()
    elif valueType == "float":
        val = FloatValue()
    elif valueType == "string":
        val = StringValue()
    elif valueType == "vec2i":
        val = Vec2iValue()
    elif valueType == "vec2":
        val = Vec2Value()
    elif valueType == "vec3":
        val = Vec3Value()
    elif valueType == "vec4":
        val = Vec4Value()
    elif valueType == "quat":
        val = QuatValue()
    elif valueType == "color":
        val = ColorValue()
    elif valueType == "entity":
        val = EntityValue()
    elif len(valueType) > 9 and valueType[:9] == "resource.":
        val = ResourceValue()
        val._resType = ResourceType.getResourceType(valueType[9:])
    else:
        valueModel = _getReflectModel().getTypeModel(valueType)
        if valueModel is None:
            raise RuntimeError("Can't find type model for a type '{0}'".format(valueType))
        valModelType = valueModel["type"]
        if valModelType == "class":
            val = ObjectValue()
            for itemName in valueModel["data"]:
                itemType = valueModel["data"][itemName]
                itemVal = _createValue(itemName, itemType)
                val._vals.append(itemVal)
        elif valModelType == "array":
            val = ArrayValue()
            elemType = _getReflectModel().getArrayElemType(valueType)
            if elemType is None:
                raise RuntimeError("Can't find element type of array: '{0}'".format(valueType))
            val._elemType = elemType
        elif valModelType == "enum":
            val = EnumValue()
            val._table = _getReflectModel().getEnumTable(valueType)
            if val._table is None:
                raise RuntimeError("Can't find enum table for enum: '{0}'".format(valueType))
        else:
            raise RuntimeError("Unknown value type: '{0}'".format(valueType))
    val._name = valueName
    return val

def CreateObjectValue(rootValue, valueType):
    valueModel = _getReflectModel().getTypeModel(valueType)
    if valueModel["type"] != "class":
        raise RuntimeError("Invalid value type")
    if type(rootValue) is not ObjectValue:
        raise RuntimeError("Invalid type of root value")
    if "data" not in valueModel:
        return True
    for valueName in valueModel["data"]:
        valueType = valueModel["data"][valueName]
        value = _createValue(valueName, valueType)
        if value is None:
            print("[createObjectValue] Can't create value '{0}' of type '{1}'".format(
                valueName, valueType))
            return False
        rootValue._vals.append(value)
    return True

def _assignValueIds(rootValue, startIdx):
    currentIdx = startIdx
    for val in rootValue._vals:
        if val._type != ValueType.Object:
            val._valueId = currentIdx
            currentIdx += 1
        else:
            currentIdx = _assignValueIds(val, currentIdx)
    return currentIdx

def AssignValueLogic(rootValue, logic):
    for val in rootValue._vals:
        if val._type != ValueType.Object:
            val._logic = logic
        else:
            AssignValueLogic(val, logic)

def AssignValueIdx(rootValue):
    _assignValueIds(rootValue, 1)