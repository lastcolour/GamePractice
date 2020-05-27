import EditorTest

from native.LibraryNative import LibraryNative
from utils.AppConfig import AppConfig

import unittest
import json

class EditorAppTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        appConfig = AppConfig()
        cls.NATIVE_LIB = LibraryNative()
        res = cls.NATIVE_LIB.initialize(appConfig.getNativeLibPath())
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
        buffer = bytearray(10 * 10 * 4)
        EditorAppTests.NATIVE_LIB.drawFrame(buffer, 10, 10)

    def testAddRemoveLogic(self):
        logicId = EditorAppTests.NATIVE_LIB.addLogicToEntity(self._centralEntityId, "RenderSimple")

        self.assertNotEqual(logicId, -1)

        EditorAppTests.NATIVE_LIB.removeLogicFromEntity(self._centralEntityId, logicId)

    def testSetGetLogicData(self):
        logicId = 0
        valueId = 4
        stream = EditorAppTests.NATIVE_LIB.getEntityLogicData(self._centralEntityId, logicId)
        self.assertIsNotNone(stream)
        self.assertTrue(stream.getSize() > 0)

        matName = stream.readString()
        geomName = stream.readString()

        self.assertEqual(matName, "")
        self.assertEqual(geomName, "")

        scaleX = stream.readFloat()
        scaleY = stream.readFloat()

        self.assertEqual(scaleX, 1.0)
        self.assertEqual(scaleY, 1.0)

        r = stream.readUChar()
        g = stream.readUChar()
        b = stream.readUChar()
        a = stream.readUChar()

        self.assertEqual(r, 255)
        self.assertEqual(g, 255)
        self.assertEqual(b, 255)
        self.assertEqual(a, 255)

        stream.resetPos()
        stream.writeUChar(1)
        stream.writeUChar(2)
        stream.writeUChar(3)
        stream.writeUChar(4)

        EditorAppTests.NATIVE_LIB.setEntityLogicValueData(self._centralEntityId, logicId, valueId, stream)
        stream = EditorAppTests.NATIVE_LIB.getEntityLogicValueData(self._centralEntityId, logicId, valueId)

        r = stream.readUChar()
        g = stream.readUChar()
        b = stream.readUChar()
        a = stream.readUChar()

        self.assertEqual(r, 1)
        self.assertEqual(g, 2)
        self.assertEqual(b, 3)
        self.assertEqual(a, 4)

if __name__ == "__main__":
    unittest.main()