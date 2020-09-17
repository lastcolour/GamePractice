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
        EditorAppTests.NATIVE_LIB.unloadAll()

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

        voidEntId = EditorAppTests.NATIVE_LIB.loadEntity("Game/Void.json")
        self.assertNotEqual(voidEntId, 0)

        childId = EditorAppTests.NATIVE_LIB.addChildEntityToEntity(self._centralEntityId, voidEntId)
        self.assertNotEqual(childId, -1)

        copyVoidId = EditorAppTests.NATIVE_LIB.getEntityChildEntityId(self._centralEntityId, childId)
        self.assertEqual(copyVoidId, voidEntId)

        EditorAppTests.NATIVE_LIB.removeChildEntityFromEntity(self._centralEntityId, voidEntId)

    def testAddRemoveLogic(self):
        logicId = EditorAppTests.NATIVE_LIB.addLogicToEntity(self._centralEntityId, "RenderSimple")

        self.assertNotEqual(logicId, -1)
        self.assertNotEqual(logicId, 0)

        EditorAppTests.NATIVE_LIB.removeLogicFromEntity(self._centralEntityId, logicId)

    def testAddChildEntity(self):
        childId = EditorAppTests.NATIVE_LIB.createChildEntity(self._centralEntityId, "TestEntity")
        self.assertNotEqual(childId, -1)
        childEntId = EditorAppTests.NATIVE_LIB.getEntityChildEntityId(self._centralEntityId, childId)
        self.assertNotEqual(childEntId, 0)

if __name__ == "__main__":
    unittest.main()