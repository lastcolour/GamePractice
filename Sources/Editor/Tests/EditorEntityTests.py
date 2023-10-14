import EditorTest

from native.EditorNative import EditorNative
from native.EntityNativeLoader import EntityNativeLoader
from native.ValueNative import *
from native.EntityNative import EntityNative
from native.EntityNativeLoader import _DEFAULT_TRANSFORM
from native.MemoryStream import MemoryStream

from utils.AppConfig import AppConfig
from view.values.EditQuatValue import _convertToEualerAngles, _convertFromEualerAngles

import unittest
import json
import math

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

    def testAllLogicMemoryLayout(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        self.assertTrue(entity.isLoadedToNative())
        moduleLogics = self._getEditor().getReflectModel().getAllRegisteredLogics()
        logics = []
        for item in moduleLogics:
            logics.extend(moduleLogics[item])
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

    def testIfModifiedAfterAddRemoveLogic(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        self.assertFalse(entity.isModified())
        logic = entity.addLogic("RenderSimple")
        self.assertTrue(entity.isModified())
        entity.removeLogic(logic.getNativeId())
        self.assertTrue(entity.isModified())

    def testIfModifiedAfterAddRemoveChild(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        childEntity = entity.addChildEntity("Entities/Game/Void.json")
        self.assertTrue(entity.isModified())
        entity.removeChildEntity(childEntity)
        self.assertTrue(entity.isModified())

    def testSaveEntity(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Simple.json")
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

    def testCreateInternalChild(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Void.json")
        self.assertTrue(entity.loadToNative())
        childEntity = entity.createNewInternalChild("Test")
        tmLogic = childEntity.getTransformLogic()
        res = {}
        tmLogic.writeToDict(res)
        self.assertEqual(res["data"], _DEFAULT_TRANSFORM)
        self.assertIsNotNone(childEntity)
        self.assertTrue(childEntity.isInternal())
        self.assertTrue(childEntity.rename("NewTest"))
        self.assertEqual(childEntity.getName(), "NewTest")

    def testCreateChildFromData(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Void.json")
        self.assertTrue(entity.loadToNative())

        self.assertEqual(len(entity.getChildren()), 0)
        voidData = entity.dumpToDict()

        firstChild = entity.addChildEntityFromData("void", voidData)
        self.assertIsNotNone(firstChild)

        secondChild = entity.addChildEntityFromData("void", voidData)
        self.assertIsNotNone(secondChild)

        self.assertNotEqual(firstChild.getName(), secondChild.getName())

        res = entity.dumpToDict()
        self.assertEqual(len(res['children']), 2)

    def testChangeEntityRotation(self):
        entity = self._getEntityLoader().loadEntity("Entities/Game/Void.json")
        self.assertTrue(entity.loadToNative())
        rotValue = None
        for v in entity._tmLogic.getValues():
            if v.getName() == "rot":
                rotValue = v
                break
        self.assertIsNotNone(rotValue)
        x, y, z, w = rotValue.getVal()

        yaw, pitch, roll = _convertToEualerAngles(x, y, z, w)
        roll += math.radians(1)

        x, y, z, w = _convertFromEualerAngles(yaw, pitch, roll)
        rotValue.setVal(x, y, z, w)

        x, y, z, w = rotValue.getVal()
        newYaw, newPitch, newRoll = _convertToEualerAngles(x, y, z, w)

        self.assertAlmostEqual(newYaw, yaw)
        self.assertAlmostEqual(newPitch, pitch)
        self.assertAlmostEqual(newRoll, roll)

if __name__ == "__main__":
    unittest.main()