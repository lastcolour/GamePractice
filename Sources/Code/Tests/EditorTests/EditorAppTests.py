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

    def setUp(self):
        retCode = EditorAppTests.EDITOR_LIB.Initiliaze()
        self.assertEqual(retCode, 0)

    def tearDown(self):
        EditorAppTests.EDITOR_LIB.DeInitialize()

    def testExportMethods(self):
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.Initiliaze)
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.GetReflectModel)
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.DeInitialize)
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.LoadEntity)
        self.assertIsNotNone(EditorAppTests.EDITOR_LIB.UnloadEntity)

    def testGetReflectModel(self):
        GetReflectModelFunc = EditorAppTests.EDITOR_LIB.GetReflectModel
        GetReflectModelFunc.restype = ctypes.c_char_p

        reflectModel = GetReflectModelFunc()
        model = json.loads(reflectModel)

        self.assertGreater(len(model), 0)

        self.assertIsNotNone(reflectModel)

    def testLoadUnloadEntity(self):
        LoadEntityFunc = EditorAppTests.EDITOR_LIB.LoadEntity
        LoadEntityFunc.argstype = [ctypes.c_char_p]
        LoadEntityFunc.restye = ctypes.c_int32
        entityName = ctypes.c_char_p("Game/Simple.json".encode('ascii'))
        entityId = LoadEntityFunc(entityName)
        self.assertNotEqual(entityId, 0)

        UnloadEntityFunc = EditorAppTests.EDITOR_LIB.UnloadEntity
        UnloadEntityFunc.argstype = [ctypes.c_int32]
        UnloadEntityFunc(entityId)

if __name__ == "__main__":
    unittest.main()