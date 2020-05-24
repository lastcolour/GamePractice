import EditorTest

import unittest

from Native.ValueNative import *

class LogicValueTests(unittest.TestCase):
    def testEntityElemBoolValue(self):
        v = BoolValue()
        v.setVal(True)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [True,])
        v.readFromDict(False)
        self.assertEqual(v.getVal(), False)

    def testEntityBoolValue(self):
        v = BoolValue()
        v.setVal(True)
        v._name = "bool"
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"bool":True})
        v.readFromDict({"bool":False})
        self.assertEqual(v.getVal(), False)

    def testEntityElemIntValue(self):
        v = IntValue()
        v.setVal(1)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [1,])
        v.readFromDict(2)
        self.assertEqual(v.getVal(), 2)

    def testEntityIntValue(self):
        v = IntValue()
        v.setVal(1)
        v._name = "int"
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"int":1})
        v.readFromDict({"int":2})
        self.assertEqual(v.getVal(), 2)

    def testEntityElemFloatValue(self):
        v = FloatValue()
        v.setVal(1.0)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [1.0,])
        v.readFromDict(2.0)
        self.assertEqual(v.getVal(), 2.0)

    def testEntityFloatValue(self):
        v = FloatValue()
        v.setVal(1.0)
        v._name = "float"
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"float":1.0})
        v.readFromDict({"float":2.0})
        self.assertEqual(v.getVal(), 2.0)

    def testEntityElemStringValue(self):
        v = StringValue()
        v.setVal("1")
        res = []
        v.writeToDict(res)
        self.assertEqual(res, ["1",])
        v.readFromDict("2")
        self.assertEqual(v.getVal(), "2")

    def testEntityStringValue(self):
        v = StringValue()
        v.setVal("1")
        v._name = "str"
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"str":"1"})
        v.readFromDict({"str":"2"})
        self.assertEqual(v.getVal(), "2")

    def testEntityElemVec2iValue(self):
        v = Vec2iValue()
        v.setVal(1, 2)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [{"x":1, "y":2},])
        v.readFromDict({"x":3, "y":4})
        x, y = v.getVal()
        self.assertEqual(x, 3)
        self.assertEqual(y, 4)

    def testEntityVec2iValue(self):
        v = Vec2iValue()
        v._name = "vec2i"
        v.setVal(1, 2)
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"vec2i": {"x":1, "y":2}})
        v.readFromDict({"vec2i": {"x":3, "y":4}})
        x, y = v.getVal()
        self.assertEqual(x, 3)
        self.assertEqual(y, 4)

    def testEntityElemVec2Value(self):
        v = Vec2Value()
        v.setVal(1.5, 2.5)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [{"x":1.5, "y":2.5},])
        v.readFromDict({"x":3.5, "y":4.5})
        x, y = v.getVal()
        self.assertEqual(x, 3.5)
        self.assertEqual(y, 4.5)

    def testEntityVec2Value(self):
        v = Vec2Value()
        v._name = "vec2"
        v.setVal(1.5, 2.5)
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"vec2": {"x":1.5, "y":2.5}})
        v.readFromDict({"vec2": {"x":3.5, "y":4.5}})
        x, y = v.getVal()
        self.assertEqual(x, 3.5)
        self.assertEqual(y, 4.5)

    def testEntityElemVec3Value(self):
        v = Vec3Value()
        v.setVal(1.5, 2.5, 3.5)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [{"x":1.5, "y":2.5, "z":3.5},])
        v.readFromDict({"x":4.5, "y":5.5, "z":6.5})
        x, y, z = v.getVal()
        self.assertEqual(x, 4.5)
        self.assertEqual(y, 5.5)
        self.assertEqual(z, 6.5)

    def testEntityVec3Value(self):
        v = Vec3Value()
        v._name = "vec3"
        v.setVal(1.5, 2.5, 3.5)
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"vec3": {"x":1.5, "y":2.5, "z":3.5}})
        v.readFromDict({"vec3": {"x":4.5, "y":5.5, "z": 6.5}})
        x, y, z = v.getVal()
        self.assertEqual(x, 4.5)
        self.assertEqual(y, 5.5)
        self.assertEqual(z, 6.5)

    def testEntityElemVec4Value(self):
        v = Vec4Value()
        v.setVal(1.5, 2.5, 3.5, 4.5)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [{"x":1.5, "y":2.5, "z":3.5, "w":4.5},])
        v.readFromDict({"x":5.5, "y":6.5, "z":7.5, "w":8.5})
        x, y, z, w = v.getVal()
        self.assertEqual(x, 5.5)
        self.assertEqual(y, 6.5)
        self.assertEqual(z, 7.5)
        self.assertEqual(w, 8.5)

    def testEntityVec4Value(self):
        v = Vec4Value()
        v._name = "vec4"
        v.setVal(1.5, 2.5, 3.5, 4.5)
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"vec4": {"x":1.5, "y":2.5, "z":3.5, "w":4.5}})
        v.readFromDict({"vec4": {"x":5.5, "y":6.5, "z": 7.5, "w":8.5}})
        x, y, z, w = v.getVal()
        self.assertEqual(x, 5.5)
        self.assertEqual(y, 6.5)
        self.assertEqual(z, 7.5)
        self.assertEqual(w, 8.5)

    def testEntityElemColorValue(self):
        v = ColorValue()
        v.setVal(251, 252, 253, 254)
        res = []
        v.writeToDict(res)
        self.assertEqual(res, [{"r":251, "g":252, "b":253, "a":254},])
        v.readFromDict({"r":120, "g":121, "b":122, "a":123})
        r, g, b, a = v.getVal()
        self.assertEqual(r, 120)
        self.assertEqual(g, 121)
        self.assertEqual(b, 122)
        self.assertEqual(a, 123)

    def testEntityColorValue(self):
        v = ColorValue()
        v._name = "col"
        v.setVal(251, 252, 253, 254)
        res = {}
        v.writeToDict(res)
        self.assertEqual(res, {"col": {"r":251, "g":252, "b":253, "a":254}})
        v.readFromDict({"col": {"r":120, "g":121, "b":122, "a":123}})
        r, g, b, a = v.getVal()
        self.assertEqual(r, 120)
        self.assertEqual(g, 121)
        self.assertEqual(b, 122)
        self.assertEqual(a, 123)

if __name__ == "__main__":
    unittest.main()