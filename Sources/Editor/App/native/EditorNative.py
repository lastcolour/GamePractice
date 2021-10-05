from .LibraryNative import LibraryNative
from .EntityNativeLoader import EntityNativeLoader
from .Native import NativeObject
from .ReflectModel import ReflectModel
from .GameMonitor import GameMonitor

class EditorNative:
    def __init__(self, appConfig):
        self._appConfig = appConfig
        self._nativeLib = None
        self._reflectModel = None
        self._gameMonitor = GameMonitor(self._appConfig)
        self._entityLoader = EntityNativeLoader()

    def init(self):
        self._nativeLib = LibraryNative()
        libInitRes = self._nativeLib.initialize(self._appConfig.getNativeLibPath())
        if libInitRes != 0:
            print("[EditorNative:init] Can't initialize native library: '{0}' (Error: code={1})".format(
                self._appConfig.getNativeLibPath(), libInitRes))
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

    def getAssetsRootPath(self):
        return self._appConfig.getAssetsRootPath()

    def getEntityLoader(self):
        return self._entityLoader

    def getReflectModel(self):
        return self._reflectModel

    def getGameMonitor(self):
        return self._gameMonitor
