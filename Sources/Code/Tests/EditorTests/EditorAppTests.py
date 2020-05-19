from EditorNative import EditorNative

import unittest
import ctypes
import pathlib
import os
import json
import struct

def _getEditorLibPath():
    if os.name == "nt":
        libExt = ".dll"
        buildPlatform = "Windows"
    elif os.name == "linux":
        libExt = ""
        buildPlatform = "Linux"
    else:
        raise RuntimeError("Unsuported platform")
    buildType = "Debug"
    buildDir = "../../../../../_build"
    editorLibName = "Editor"
    cPath =  pathlib.Path("{0}/{1}/{2}/Game/{3}/{4}{5}".format(
        __file__, buildDir, buildPlatform, buildType, editorLibName, libExt)).resolve().__str__()
    return cPath.replace("\\", "/")

def _getPyStr(cString):
    return cString.decode('ascii').rstrip('\x00')

class EditorAppTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.NATIVE_EDITOR = EditorNative(_getEditorLibPath())
        res = cls.NATIVE_EDITOR.initialize()
        if res != 0:
            raise RuntimeError("Can't initialize native editor")

    @classmethod
    def tearDownClass(cls):
        cls.NATIVE_EDITOR.deinitialize()

    def setUp(self):
        self._centralEntityId = EditorAppTests.NATIVE_EDITOR.loadEntity("Game/Simple.json")
        self.assertNotEqual(self._centralEntityId, 0)

    def tearDown(self):
        EditorAppTests.NATIVE_EDITOR.unloadEntity(self._centralEntityId)

    def testGetReflectModel(self):
        reflectModel = EditorAppTests.NATIVE_EDITOR.getReflectModel()
        model = json.loads(reflectModel)
        self.assertGreater(len(model), 0)

    def testEntityAndChildrenName(self):
        entityName = EditorAppTests.NATIVE_EDITOR.getEntityName(self._centralEntityId)
        self.assertEqual(entityName, "Game/Simple.json")

        children = EditorAppTests.NATIVE_EDITOR.getEntityChildren(self._centralEntityId)
        self.assertEqual(len(children), 0)

    def testDrawFrame(self):
        EditorAppTests.NATIVE_EDITOR.drawFrame(10, 10)

    def testAddRemoveLogic(self):
        logicId = EditorAppTests.NATIVE_EDITOR.addLogicToEntity(self._centralEntityId, "RenderSimple")

        self.assertNotEqual(logicId, -1)

        EditorAppTests.NATIVE_EDITOR.removeLogicFromEntity(self._centralEntityId, logicId)

    def testSetGetLogicData(self):
        logicId = 0
        res = EditorAppTests.NATIVE_EDITOR.getEntityLogicData(self._centralEntityId, logicId)
        matName, geomName, scaleX, scaleY, r, g, b, a = struct.unpack_from("@ssffBBBB", res)

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

if __name__ == "__main__":
    unittest.main()