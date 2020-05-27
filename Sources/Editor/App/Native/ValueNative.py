from .Native import NativeObject
from .MemoryStream import MemoryStream

import collections

def _getReflectModel():
    return NativeObject._NATIVE_API.getReflectModel()

class ValueType:
    Bool = 0
    Int = 1
    Float = 2
    String = 3
    Vec2i = 4
    Vec2 = 5
    Vec3 = 6
    Vec4 = 7
    Color = 8
    Enum = 9
    Resource = 10
    Entity = 11
    Object = 12
    Array = 13

class ValueNative(NativeObject):
    def __init__(self, valueType):
        self._name = None
        self._logic = None
        self._valueId = None
        self._type = valueType

    def getName(self):
        return self._name

    def getType(self):
        return self._type

    def getPrimitiveValueCount(self):
        return 1

    def _getLogicId(self):
        return self._logic.getNativeId()

    def _getEntityId(self):
        return self._logic.getEntity().getNativeId()

    def _isLoadedToNative(self):
        if self._logic is None:
            return False
        return self._logic.getEntity().isLoadedToNative()

    def _writeToNative(self):
        if self.getPrimitiveValueCount() != 1:
            raise RuntimeError("Can't write to native non-primitive values")
        if not self._isLoadedToNative():
            return
        stream = MemoryStream()
        self.writeToStream(stream)
        self._getAPI().getLibrary().setEntityLogicValueData(self._getEntityId(), self._getLogicId(), self._valueId, stream)

class BoolValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Bool)
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            self._val = bool(node[self._name])
        else:
            self._val = bool(node)

    def writeToDict(self, node):
        if self._name is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readBool()

    def writeToStream(self, stream):
        stream.writeBool(self._val)

    def setVal(self, flag):
        self._val = bool(flag)
        self._writeToNative()

    def getVal(self):
        return self._val

class IntValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Int)
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            self._val = int(node[self._name])
        else:
            self._val = int(node)

    def writeToDict(self, node):
        if self._name is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readInt()

    def writeToStream(self, stream):
        stream.writeInt(self._val)

    def setVal(self, val):
        self._val = int(val)
        self._writeToNative()

    def getVal(self):
        return self._val

class FloatValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Float)
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            self._val = float(node[self._name])
        else:
            self._val = float(node)

    def writeToDict(self, node):
        if self._name is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readFloat()

    def writeToStream(self, stream):
        stream.writeFloat(self._val)

    def setVal(self, val):
        self._val = float(val)
        self._writeToNative()

    def getVal(self):
        return self._val

class StringValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.String)
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            self._val = str(node[self._name])
        else:
            self._val = str(node)

    def writeToDict(self, node):
        if self._name is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readString()

    def writeToStream(self, stream):
        stream.writeString(self._val)

    def setVal(self, val):
        self._val = str(val)
        self._writeToNative()

    def getVal(self):
        return self._val

class Vec2iValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec2i)
        self._xVal = None
        self._yVal = None

    def readFromDict(self, node):
        if self._name is not None:
            self._xVal = int(node[self._name]["x"])
            self._yVal = int(node[self._name]["y"])
        else:
            self._xVal = int(node["x"])
            self._yVal = int(node["y"])

    def writeToDict(self, node):
        if self._name is not None:
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
        self._writeToNative()

    def getVal(self):
        return self._xVal, self._yVal

class Vec2Value(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec2)
        self._xVal = None
        self._yVal = None

    def readFromDict(self, node):
        if self._name is not None:
            self._xVal = float(node[self._name]["x"])
            self._yVal = float(node[self._name]["y"])
        else:
            self._xVal = float(node["x"])
            self._yVal = float(node["y"])

    def writeToDict(self, node):
        if self._name is not None:
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
        self._writeToNative()

    def getVal(self):
        return self._xVal, self._yVal

class Vec3Value(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Vec3)
        self._xVal = None
        self._yVal = None
        self._zVal = None

    def readFromDict(self, node):
        if self._name is not None:
            self._xVal = float(node[self._name]["x"])
            self._yVal = float(node[self._name]["y"])
            self._zVal = float(node[self._name]["z"])
        else:
            self._xVal = float(node["x"])
            self._yVal = float(node["y"])
            self._zVal = float(node["z"])

    def writeToDict(self, node):
        if self._name is not None:
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
        self._writeToNative()

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
        if self._name is not None:
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
        if self._name is not None:
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
        self._writeToNative()


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
        if self._name is not None:
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
        if self._name is not None:
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
        self._writeToNative()

    def getVal(self):
        return self._rVal, self._gVal, self._bVal, self._aVal

class ArrayValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Array)
        self._val = None
        self._elemCls = None

    def readFromDict(self, node):
        if self._name is None:
            raise RuntimeError("Array of arrays is not supported")
        self._val = []
        for elemNode in node[self._name]:
            elem = self._elemCls()
            elem.readFromDict(elemNode)
            self._val.append(elem)

    def writeToDict(self, node):
        if self._name is None:
            raise RuntimeError("Array of arrays is not supported")
        resList = []
        for item in self._val:
            item.writeToDict(resList)
        node[self._name] = resList

    def readFromStream(self, stream):
        self._val = []
        size = stream.readInt()
        for i in range(size):
            elem = self._elemCls()
            elem.readFromStream(stream)
            self._val.append(elem)

    def writeToStream(self, stream):
        stream.writeInt(len(self._val))
        for elem in self._val:
            elem.writeToStream(stream)

class EnumValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Enum)
        self._table = {}
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            strVal = str(node[self._name])
        else:
            strVal = str(node)
        self._val = int(self._table[strVal])

    def writeToDict(self, node):
        for item in self._table:
            if self._table[item] == self._val:
                if self._name is not None:
                    node[self._name] = str(item)
                else:
                    node.append(str(item))
                return
        raise RuntimeError("Can't find enum value in table")

    def readFromStream(self, stream):
        self._val = stream.readInt()

    def writeToStream(self, stream):
        stream.writeInt(self, stream)

    def setVal(self, val):
        self._val = int(self._table[str(val)])
        self._writeToNative()

    def getVal(self):
        for item in self._table:
            if self._table[item] == self._val:
                return item
        raise RuntimeError("Can't find enum value in table")

class ObjectValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Object)
        self._vals = []

    def readFromDict(self, node):
        objectNode = node
        if self._name is not None:
            objectNode = node[self._name]
        for val in self._vals:
            val.readFromDict(objectNode)

    def writeToDict(self, node):
        objectNode = {}
        for val in self._vals:
            val.writeToDict(objectNode)
        if self._name is not None:
            node[self._name] = objectNode
        else:
            node.append(objectNode)

    def readFromStream(self, stream):
        for item in self._vals:
            item.readFromStream(stream)

    def writeToStream(self, stream):
        for item in self._vals:
            item.writeToStream(stream)

    def getPrimitiveValueCount(self):
        total = 0
        for item in self._vals:
            total += self._vals[item].getPrimitiveValueCount()
        return total

class ResourceValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Resource)
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            self._val = str(node[self._name])
        else:
            self._val = str(node)

    def writeToDict(self, node):
        if self._val is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readString()

    def writeToStream(self, stream):
        stream.writeString(self._val)

    def setVal(self, val):
        self._val = str(val)
        self._writeToNative()

    def getVal(self):
        return self._val

class EntityValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Entity)
        self._val = None

    def readFromDict(self, node):
        if self._name is not None:
            self._val = str(node[self._name])
        else:
            self._val = str(node)

    def writeToDict(self, node):
        if self._name is not None:
            node[self._name] = self._val
        else:
            node.append(self._val)

    def readFromStream(self, stream):
        self._val = stream.readString()

    def writeToStream(self, stream):
        stream.wrirteString(self._val)

    def setVal(self, val):
        self._val = str(val)

    def getVal(self):
        return self._val

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
    elif valueType == "color":
        val = ColorValue()
    elif valueType == "resource":
        val = ResourceValue()
    else:
        valueModel = _getReflectModel().getTypeModel(valueType)
        valueType = valueModel["type"]
        if valueType == "class":
            val = ObjectValue()
            CreateObjectValue(val, valueModel["data"])
        elif valueType == "array":
            val = ArrayValue()
            elemType =  _getReflectModel().getArrayElemType(valueType)
            val._elemCls = _createValue(None, elemType).__class__
        elif valueType == "enum":
            val = EnumValue()
            val._table = _getReflectModel().getEnumTable(valueType)
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