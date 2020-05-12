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
        cls.EDITOR_LIB_PATH = _getEditorLibPath()
        if not os.path.exists(cls.EDITOR_LIB_PATH):
            print("Can't find editor library: {0}".format(cls.EDITOR_LIB_PATH))
            raise RuntimeError("Invalid library path")
        try:
            os.chdir(pathlib.Path(cls.EDITOR_LIB_PATH).parent.__str__())
            cls.EDITOR_LIB = ctypes.CDLL(cls.EDITOR_LIB_PATH)
        except:
            print("Can't load library: {0}".format(cls.EDITOR_LIB_PATH))
            raise

    def testExportMethods(self):
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.Initiliaze)
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.GetReflectModel)
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.DeInitialize)

    def testGetReflectModel(self):
        retCode = EditorAppTests.EDITOR_LIB.Initiliaze()
        self.assertEqual(retCode, 0)
        reflectModel = EditorAppTests.EDITOR_LIB.GetReflectModel()
        self.assertIsNotNone(reflectModel)
        EditorAppTests.EDITOR_LIB.DeInitialize()

if __name__ == "__main__":
    unittest.main()