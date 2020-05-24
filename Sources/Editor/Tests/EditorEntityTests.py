import EditorTest

from Native.EditorNative import EditorNative
from Native.EntityNativeLoader import EntityNativeLoader
from Native.ValueNative import *
from Native.EntityNative import EntityNative

import unittest

class EditorEntityTest(unittest.TestCase):

    EDITOR = None

    @classmethod
    def setUpClass(cls):
        cls.EDITOR = EditorNative()
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

        logics = entity.getLogics()
        self.assertGreater(len(logics), 0)

        logic = logics[0]
        self.assertEqual(logic.getName(), "RenderSimple")
        self.assertEqual(logic._logicId, 0)

        values = logic.getValues()
        self.assertEqual(len(values), 4)

        self.assertEqual(values[0].getName(), "geometry")
        self.assertEqual(values[0].getType(), ValueType.Resource)
        self.assertEqual(values[0]._valueId, 1)

        self.assertEqual(values[1].getName(), "material")
        self.assertEqual(values[1].getType(), ValueType.Resource)
        self.assertEqual(values[1]._valueId, 2)

        self.assertEqual(values[2].getName(), "scale")
        self.assertEqual(values[2].getType(), ValueType.Vec2)
        self.assertEqual(values[2]._valueId, 3)

        self.assertEqual(values[3].getName(), "color")
        self.assertEqual(values[3].getType(), ValueType.Color)
        self.assertEqual(values[3]._valueId, 4)

    def testAllLogicMemoryLayout(self):
        entity = self._getEntityLoader().loadEntity("Game/Simple.json")
        self.assertTrue(entity.loadToNative())
        self.assertTrue(entity.isLoadedToNative())
        logics = self._getEditor().getReflectModel().getAllRegisteredLogic()
        for logicName in logics:
            if logicName.startswith("UI"):
                continue
            logic = entity.addLogic(logicName)
            self.assertIsNotNone(logic)
            entity.removeLogic(logic.getNativeId())

if __name__ == "__main__":
    unittest.main()