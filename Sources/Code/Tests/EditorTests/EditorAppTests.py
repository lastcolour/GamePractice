from EditorNative import EditorNative

import unittest
import ctypes
import pathlib
import os
import json

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

    def testGetReflectModel(self):
        reflectModel = EditorAppTests.NATIVE_EDITOR.getReflectModel()
        model = json.loads(reflectModel)
        self.assertGreater(len(model), 0)

    def testLoadUnloadEntity(self):
        entityId = EditorAppTests.NATIVE_EDITOR.loadEntity("Game/Simple.json")
        self.assertNotEqual(entityId, 0)

        entityName = EditorAppTests.NATIVE_EDITOR.getEntityName(entityId)
        self.assertEqual(entityName, "Game/Simple.json")

        children = EditorAppTests.NATIVE_EDITOR.getEntityChildren(entityId)
        self.assertEqual(len(children), 0)

        EditorAppTests.NATIVE_EDITOR.unloadEntity(entityId)

    def testDrawFrame(self):
        entityId = EditorAppTests.NATIVE_EDITOR.loadEntity("Game/Simple.json")
        self.assertNotEqual(entityId, 0)

        EditorAppTests.NATIVE_EDITOR.drawFrame(10, 10)

        EditorAppTests.NATIVE_EDITOR.unloadEntity(entityId)

    def testAddRemoveLogic(self):
        entityId = EditorAppTests.NATIVE_EDITOR.loadEntity("Game/Simple.json")

        logicId = EditorAppTests.NATIVE_EDITOR.addLogicToEntity(entityId, "RenderSimple")

        self.assertNotEqual(logicId, -1)

        EditorAppTests.NATIVE_EDITOR.removeLogicFromEntity(entityId, logicId)

        EditorAppTests.NATIVE_EDITOR.unloadEntity(entityId)

if __name__ == "__main__":
    unittest.main()