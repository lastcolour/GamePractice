import EditorTest

from native.EditorNative import EditorNative
from native.EntityNativeLoader import EntityNativeLoader
from native.ValueNative import *
from native.EntityNative import EntityNative

from utils.AppConfig import AppConfig

import unittest
import json

class EditorEntityTest(unittest.TestCase):

    EDITOR = None

    @classmethod
    def setUpClass(cls):
        cls.EDITOR = EditorNative(AppConfig())
        if not cls.EDITOR.init():
            cls.EDITOR = None
            raise RuntimeError("Can't init editor native")

    @classmethod
    def tearDownClass(cls):
        if cls.EDITOR is not None:
            cls.EDITOR.deinit()
            cls.EDITOR = None

    def _getEditor(self):
        return EditorEntityTest.EDITOR

    def _getEntityLoader(self):
        return self._getEditor().getEntityLoader()

    def testLoadEntity(self):
        loader = self._getEntityLoader()
        entity = loader.loadEntity("Game/Simple.json")
        self.assertIsNotNone(entity)
        self.assertEqual(entity.getName(), "Game/Simple.json")

        logics = entity.getLogics()
        self.assertGreater(len(logics), 0)

        logic = logics[0]
        self.assertEqual(logic.getName(), "RenderSimple")
        self.assertEqual(logic._logicId, 1)

        values = logic.getValues()
        self.assertEqual(len(values), 2)

        self.assertEqual(values[0].getName(), "size")
        self.assertEqual(values[0].getType(), ValueType.Vec2i)
        self.assertEqual(values[0]._valueId, 1)

        self.assertEqual(values[1].getName(), "color")
        self.assertEqual(values[1].getType(), ValueType.Color)
        self.assertEqual(values[1]._valueId, 2)

    def testAllLogicMemoryLayout(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        self.assertTrue(entity.isLoadedToNative())
        logics = self._getEditor().getReflectModel().getAllRegisteredLogics()
        for logicName in logics:
            logic = entity.addLogic(logicName)
            self.assertIsNotNone(logic)
            native = self._getEditor().getLibrary().getEntityLogicData(entity.getNativeId(), logic.getNativeId(), 0)
            self.assertIsNotNone(native)
            current = MemoryStream()
            logic.writeToStream(current)
            self.assertEqual(current.tellg(), native.getSize())
            self.assertEqual(current._data[:native.getSize()], native._data)
            entity.removeLogic(logic.getNativeId())

    def testModifyLogicValue(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        self.assertTrue(entity.isLoadedToNative())
        renderLogic = entity.getLogics()[0]
        colorVal = renderLogic.getValues()[1]
        colorVal.setVal(123, 124, 125, 126)

        stream = self._getEditor().getLibrary().getEntityLogicData(entity.getNativeId(), renderLogic.getNativeId(), 2)
        r = stream.readUChar()
        g = stream.readUChar()
        b = stream.readUChar()
        a = stream.readUChar()

        self.assertEqual(r, 123)
        self.assertEqual(g, 124)
        self.assertEqual(b, 125)
        self.assertEqual(a, 126)

    def testIfModifiedAfterAddRemoveLogic(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        self.assertFalse(entity.isModified())
        logic = entity.addLogic("RenderSimple")
        self.assertTrue(entity.isModified())
        entity.removeLogic(logic.getNativeId())
        self.assertTrue(entity.isModified())

    def testIfModifiedAfterAddRemoveChild(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        childEntity = entity.addChildEntity("Game/Void.json")
        self.assertTrue(entity.isModified())
        entity.removeChildEntity(childEntity)
        self.assertTrue(entity.isModified())

    def testSaveEntity(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        res = entity.dumpToDict()
        self.assertIsNotNone(res)
        self.assertIn("children", res)
        self.assertIn("logics", res)

        fileData = None
        with open(entity.getFullFilePath(), 'r') as tFile:
            fileData = json.load(tFile)

        self.assertEqual(res, fileData)

        self.assertTrue(entity.loadToNative())
        logic = entity.addLogic("RenderSimple")

        self.assertTrue(entity.isModified())
        entity.save()
        self.assertFalse(entity.isModified())

        fileData = None
        with open(entity.getFullFilePath(), 'r') as tFile:
            fileData = json.load(tFile)

        self.assertNotEqual(res, fileData)

        entity.removeLogic(logic.getNativeId())

        self.assertTrue(entity.isModified())
        entity.save()
        self.assertFalse(entity.isModified())

        fileData = None
        with open(entity.getFullFilePath(), 'r') as tFile:
            fileData = json.load(tFile)

        self.assertEqual(res, fileData)

    def testLogicWithArray(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        logic = entity.addLogic("UILayout")
        self.assertIsNotNone(logic)
        values = logic.getValues()
        self.assertTrue(len(values) > 1)
        arrayVal = values[1]
        self.assertEqual(arrayVal.getType(), ValueType.Array)
        elemCount = arrayVal.getValues()
        self.assertEqual(len(elemCount), 0)
        newElemVal = arrayVal.addNewElement()
        self.assertIsNotNone(newElemVal)
        self.assertEqual(newElemVal.getType(), ValueType.Entity)
        self.assertEqual(newElemVal.getName(), "[0]")
        arrayVal.removeElement(0)
        elemCount = arrayVal.getValues()
        self.assertEqual(len(elemCount), 0)

    def testCreateInternalChild(self):
        entity = self._getEntityLoader().loadEntity("Game/Void.json")
        self.assertTrue(entity.loadToNative())
        childEntity = entity.createNewInternalChild("Test")
        self.assertIsNotNone(childEntity)
        self.assertTrue(childEntity.isInternal())

if __name__ == "__main__":
    unittest.main()