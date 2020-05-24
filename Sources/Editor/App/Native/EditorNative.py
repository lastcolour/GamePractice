import os
import pathlib
import json

from .LibraryNative import LibraryNative
from .EntityNativeLoader import EntityNativeLoader
from .Native import NativeObject
from .ReflectModel import ReflectModel

def _getRootDir():
    cPath = pathlib.Path("{0}/../../../../../..".format(__file__)).parent
    return cPath.resolve().__str__()

def _getNativeLibPath(buildType):
    if os.name == "nt":
        libExt = ".dll"
        buildPlatform = "Windows"
    elif os.name == "linux":
        libExt = ""
        buildPlatform = "Linux"
    else:
        raise RuntimeError("Unsuported platform")
    buildDir = "{0}/_build".format(_getRootDir())
    editorLibName = "Editor"
    cPath =  pathlib.Path("{0}/{1}/Game/{2}/{3}{4}".format(
        buildDir, buildPlatform, buildType, editorLibName, libExt)).__str__()
    return cPath.replace("\\", "/")

class EditorNative:
    def __init__(self):
        self._libBuildType = "Debug"
        self._nativeLib = None
        self._reflectModel = None
        self._entityLoader = EntityNativeLoader()

    def init(self):
        libPath = _getNativeLibPath(self._libBuildType)
        if not os.path.exists(libPath):
            print("[EditorNative:init] Can't find require native library: '{0}'".format(libPath))
            return False
        self._nativeLib = LibraryNative()
        libInitRes = self._nativeLib.initialize(libPath)
        if libInitRes != 0:
            print("[EditorNative:init] Can't initialize native library: '{0}' (Error: code={1})".format(
                libPath, libInitRes))
            return False
        NativeObject._NATIVE_API = self
        self._reflectModel = ReflectModel()
        if not self._reflectModel.init():
            print("[EditorNative:init] Can't initialize reflect model")
            return False
        self._entityLoader = EntityNativeLoader()
        if not self._entityLoader.init():
            print("[EditorNative:init] Can't initialize entity loader")
            return False
        return True

    def deinit(self):
        if self._nativeLib is not None:
            self._nativeLib.deinitialize()
            self._nativeLib = None

    def getLibrary(self):
        return self._nativeLib

    def getAssetsRootDir(self):
        return "{0}/Assets".format(_getRootDir())

    def getEntityLoader(self):
        return self._entityLoader

    def getReflectModel(self):
        return self._reflectModel
