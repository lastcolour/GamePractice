import EditorTest

from Native.LibraryNative import LibraryNative
from Native.EditorNative import _getNativeLibPath

import unittest
import ctypes
import pathlib
import os
import json
import struct

def _getPyStr(cString):
    return cString.decode('ascii').rstrip('\x00')

class EditorAppTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.NATIVE_LIB = LibraryNative()
        res = cls.NATIVE_LIB.initialize(_getNativeLibPath("Debug"))
        if res != 0:
            raise RuntimeError("Can't initialize native editor")

    @classmethod
    def tearDownClass(cls):
        cls.NATIVE_LIB.deinitialize()

    def setUp(self):
        self._centralEntityId = EditorAppTests.NATIVE_LIB.loadEntity("Game/Simple.json")
        self.assertNotEqual(self._centralEntityId, 0)

    def tearDown(self):
        EditorAppTests.NATIVE_LIB.unloadEntity(self._centralEntityId)

    def testGetReflectModel(self):
        reflectModel = EditorAppTests.NATIVE_LIB.getReflectModel()
        model = json.loads(reflectModel)
        self.assertGreater(len(model), 0)

    def testGetRegisteredEntityLogics(self):
        registeredLogics = EditorAppTests.NATIVE_LIB.getRegisteredEntityLogics()
        self.assertGreater(len(registeredLogics), 0)

    def testEntityAndChildrenName(self):
        entityName = EditorAppTests.NATIVE_LIB.getEntityName(self._centralEntityId)
        self.assertEqual(entityName, "Game/Simple.json")

        children = EditorAppTests.NATIVE_LIB.getEntityChildren(self._centralEntityId)
        self.assertEqual(len(children), 0)

        childEntityId = EditorAppTests.NATIVE_LIB.addChildEntityToEntity(self._centralEntityId, "Game/Simple.json")

        children = EditorAppTests.NATIVE_LIB.getEntityChildren(self._centralEntityId)
        self.assertEqual(len(children), 1)
        self.assertEqual(children[0], childEntityId)

        EditorAppTests.NATIVE_LIB.removeChildEntityFromEntity(self._centralEntityId, childEntityId)

        children = EditorAppTests.NATIVE_LIB.getEntityChildren(self._centralEntityId)
        self.assertEqual(len(children), 0)

    def testDrawFrame(self):
        EditorAppTests.NATIVE_LIB.drawFrame(10, 10)

    def testAddRemoveLogic(self):
        logicId = EditorAppTests.NATIVE_LIB.addLogicToEntity(self._centralEntityId, "RenderSimple")

        self.assertNotEqual(logicId, -1)

        EditorAppTests.NATIVE_LIB.removeLogicFromEntity(self._centralEntityId, logicId)

    def testSetGetLogicData(self):
        logicId = 0
        valueId = 4
        res = EditorAppTests.NATIVE_LIB.getEntityLogicData(self._centralEntityId, logicId)
        self.assertIsNotNone(res)
        self.assertTrue(len(res) > 0)
        matName, geomName, scaleX, scaleY, r, g, b, a = struct.unpack_from("<ssffBBBB", res)

        matName = _getPyStr(matName)
        geomName = _getPyStr(geomName)

        self.assertEqual(matName, "")
        self.assertEqual(geomName, "")

        self.assertEqual(scaleX, 1.0)
        self.assertEqual(scaleY, 1.0)

        self.assertEqual(r, 255)
        self.assertEqual(g, 255)
        self.assertEqual(b, 255)
        self.assertEqual(a, 255)

        data = bytearray(4)
        struct.pack_into("<BBBB", data, 0, 1, 2, 3, 4)

        EditorAppTests.NATIVE_LIB.setEntiyLogicValueData(self._centralEntityId, logicId, valueId, data)
        data = EditorAppTests.NATIVE_LIB.getEntityLogicValueData(self._centralEntityId, logicId, valueId)

        r, g, b, a = struct.unpack_from("<BBBB", data)

        self.assertEqual(r, 1)
        self.assertEqual(g, 2)
        self.assertEqual(b, 3)
        self.assertEqual(a, 4)

if __name__ == "__main__":
    unittest.main()