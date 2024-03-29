from .Native import NativeObject

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
    Quat = 9
    Color = 10
    Enum = 11
    Resource = 12
    Entity = 13
    Object = 14
    Array = 15
    PolymorphObject = 16

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
        logic = self.getLogic()
        if logic is not None:
            return logic.getEntity()
        return None

    def getLogic(self):
        if self._logic is not None:
            return self._logic
        if self._arrayVal is not None:
            return self._arrayVal.getLogic()
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
        self._arrayVal = arrayVal

    def _onValueChanged(self):
        if self.getPrimitiveValueCount() != 1:
            raise RuntimeError("Can't write to native non-primitive values")
        if not self._isLoadedToNative():
            return
        self._isModified = True
        self.getLogic()._onValueChanged()

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
            elem = _createValue("[{0}]".format(idx), self.getLogic(), self._elemType)
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
        idx = 0
        self._val = self._vals[:size]
        for item in self._vals:
            item.readFromStream(stream)
            item._name = "[{0}]".format(idx)
            idx += 1
        for i in range(newElemsCount):
            elem = _createValue("[{0}]".format(idx), self.getLogic(), self._elemType)
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
        self._getAPI().getLibrary().addEntityLogicArrayElement(
            self.getEntityId(), self.getLogicId(), self._valueId)
        stream = self._getAPI().getLibrary().getEntityLogicData(
            self.getEntityId(), self.getLogicId(), self._valueId)
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
    def __init__(self, typeName):
        super().__init__(ValueType.Object)
        self._typeName = typeName
        self._vals = []

    def getValues(self):
        return self._vals

    def readFromDict(self, node):
        objectNode = node
        if not self._isArrayElement and self._name is not None:
            objectNode = node[self._name]
        for val in self._vals:
            if val._name not in objectNode:
                print("[ObjectValue:readFromStream] Can't find value '{0}' data".format(val._name))
                continue
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
        self._specType = None
        self._storageVal = None
        self._isSet = False

    def readFromDict(self, node):
        if not self._isArrayElement:
            if node[self._name] != None:
                self._storageVal.readFromDict(node)
                self._isSet = True
            else:
                self._isSet = False
        else:
            if node != None:
                self._storageVal.readFromDict(node)
                self._isSet = True
            else:
                self._isSet = False

    def writeToDict(self, node):
        if not self._isArrayElement:
            if self._isSet:
                self._storageVal.writeToDict(node)
            else:
                node[self._name] = None
        else:
            if self._isSet:
               self._storageVal.writeToDict(node)
            else:
                node.append(None)

    def readFromStream(self, stream):
        self._isSet = stream.readBool()
        if self._isSet:
            self._storageVal.readFromStream(stream)

    def writeToStream(self, stream):
        stream.writeBool(self._isSet)
        if self._isSet:
            self._storageVal.writeToStream(stream)

    def getSpecType(self):
        return self._specType

    def getStorage(self):
        return self._storageVal

    def isStorageSet(self):
        return self._isSet

    def setIsSet(self, isSet):
        self._isSet = isSet
        self._onValueChanged()

class EntityValue(ValueNative):
    def __init__(self):
        super().__init__(ValueType.Entity)
        self._vals = None

    def readFromDict(self, node):
        readNode = node
        if not self._isArrayElement:
            readNode = node[self._name]
        self._vals = []
        for childId in readNode:
            self._vals.append(childId)

    def writeToDict(self, node):
        for val in self._vals:
            if val == -1:
                self._vals = []
        if not self._isArrayElement:
            node[self._name] = self._vals
        else:
            node.append(self._vals)

    def readFromStream(self, stream):
        self._vals = []
        childIds = stream.readInt()
        for i in range(childIds):
            val = stream.readInt()
            self._vals.append(val)

    def writeToStream(self, stream):
        stream.writeInt(len(self._vals))
        for val in self._vals:
            stream.writeInt(val)

    def setEntityValue(self, entity):
        self._vals = []
        if entity == None:
            pass
        elif self.getEntityId() == entity.getNativeId():
            self._vals.append(0)
        else:
            while True:
                refEntId = entity.getNativeId()
                if refEntId != self.getEntityId():
                    self._vals.append(entity._childId)
                    entity = entity.getParent()
                else:
                    break
        self._vals.reverse()
        self._onValueChanged()

    def getEntityValue(self):
        if not self._vals:
            return None
        currEntity = self.getEntity()
        for refEntId in self._vals:
            currEntity = currEntity.getChildWithId(refEntId)
            if currEntity is None:
                return None
        return currEntity

class PolymorphObject(ValueNative):
    def __init__(self):
        super().__init__(ValueType.PolymorphObject)
        self._baseType = None
        self._objVal = None

    def getAllDerivedTypes(self):
        return _getReflectModel().getAllDerivedTypes(self._baseType)

    def getValues(self):
        if self._objVal is None:
            return []
        return self._objVal.getValues()

    def getPolymorphType(self):
        if self._objVal is None:
            return "Null"
        return self._objVal._typeName

    def updatePolymorhType(self, newType):
        self._getAPI().getLibrary().setEntityLogicPolymorphObjectType(
            self.getEntityId(), self.getLogicId(), self._valueId, newType)
        stream = self._getAPI().getLibrary().getEntityLogicData(
            self.getEntityId(), self.getLogicId(), self._valueId)
        self.readFromStream(stream)

    def _setPolymorphType(self, newType):
        if newType == "Null":
            if self._objVal is None:
                return
            else:
                self._objVal = None
        else:
            if self._objVal is None or self._objVal._typeName != newType:
                self._objVal = _createValue(None, self.getLogic(), newType)

    def readFromDict(self, node):
        elemNode = node
        if not self._isArrayElement and self._name is not None:
            elemNode = node[self._name] 
        objType = elemNode["type"]
        self._setPolymorphType(objType)
        if self._objVal is not None:
            self._objVal.readFromDict(elemNode["data"])

    def writeToDict(self, node):
        data = {}
        if self._objVal is not None:
            data["type"] = self._objVal._typeName
            objData = []
            self._objVal.writeToDict(objData)
            data["data"] = objData[0]
        else:
            data["type"] = "Null"
        if not self._isArrayElement and self._name is not None:
            node[self._name] = data
        else:
            node.append(data)

    def readFromStream(self, stream):
        objType = stream.readString()
        self._setPolymorphType(objType)
        if self._objVal is not None:
            self._objVal.readFromStream(stream)

    def writeToStream(self, stream):
        if self._objVal is not None:
            stream.writeString(self._objVal._typeName)
            self._objVal.writeToStream(stream)
        else:
            stream.writeString("Null")

def _createValue(valueName, logic, valueType):
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
    elif len(valueType) > 6 and valueType[:6] == "array.":
        val = ArrayValue()
        val._elemType = valueType[6:]
    elif len(valueType) > 9 and valueType[:9] == "poly_ptr.":
        val = PolymorphObject()
        val._baseType = valueType[9:]
    elif len(valueType) > 9 and valueType[:9] == "resource.":
        valueModel = _getReflectModel().getTypeModel(valueType)
        if valueModel is None:
            raise RuntimeError("Can't find type model for a type '{0}'".format(valueType))
        val = ResourceValue()
        val._specType = ResourceType.getResourceType(valueModel["spec"])
        val._storageVal = _createValue(valueName, logic, valueModel["storage"])
    else:
        valueModel = _getReflectModel().getTypeModel(valueType)
        if valueModel is None:
            raise RuntimeError("Can't find type model for a type '{0}'".format(valueType))
        valModelType = valueModel["type"]
        if valModelType == "class":
            val = ObjectValue(valueType)
            if "data" in valueModel:
                for itemName in valueModel["data"]:
                    itemType = valueModel["data"][itemName]
                    itemVal = _createValue(itemName, logic, itemType)
                    val._vals.append(itemVal)
        elif valModelType == "enum":
            val = EnumValue()
            val._table = _getReflectModel().getEnumTable(valueType)
            if val._table is None:
                raise RuntimeError("Can't find enum table for enum: '{0}'".format(valueType))
        else:
            raise RuntimeError("Unknown value type: '{0}'".format(valueType))
    val._name = valueName
    val._logic = logic
    return val

def _assignValueIds(rootValue, startIdx):
    currentIdx = startIdx
    for val in rootValue._vals:
        if val._type != ValueType.Object:
            val._valueId = currentIdx
            currentIdx += 1
        else:
            currentIdx = _assignValueIds(val, currentIdx)
    return currentIdx

def AssignValueIdx(rootValue):
    _assignValueIds(rootValue, 1)