import os
import pathlib

class AppConfig:
    def __init__(self):
        self._buildType = "Debug"
        self._platform = self._getPlatform()
        self._projectRootPath = self._getProjectRootPath()
        self._assetsRootPath = "{0}/{1}".format(self._projectRootPath, "Assets")
        self._nativeLibName = "Editor"
        self._nativeLibPath = self._getNativeLibPath()

    def getNativeLibPath(self):
        return self._nativeLibPath

    def getAssetsRootPath(self):
        return self._assetsRootPath

    def _getPlatform(self):
        if os.name == "nt":
            return "Windows"
        elif os.name == "linux":
            return "Linux"
        raise RuntimeError("Unsuported platform")

    def _getNativeLibExt(self):
        if self._platform == "Linux":
            return ".so"
        elif self._platform == "Windows":
            return ".dll"
        raise RuntimeError("Unsuported platform")

    def _getNativeLibPath(self):
        buildDir = "{0}/_build".format(self._projectRootPath)
        cPath =  pathlib.Path("{0}/{1}/Game/{2}/{3}{4}".format(
            buildDir, self._platform, self._buildType, self._nativeLibName, self._getNativeLibExt())).__str__()
        resPath = cPath.replace("\\", "/")
        if not os.path.exists(resPath):
            raise RuntimeError("Can't find native lib path: '{0}'".format(resPath))
        return resPath

    def _getProjectRootPath(self):
        cPath = pathlib.Path("{0}/../../../../../..".format(__file__)).parent
        resPath = cPath.resolve().__str__()
        resPath = resPath.replace("\\", "/")
        if not os.path.exists(resPath):
            raise RuntimeError("Can't find project root path: '{0}'".format(resPath))
        return resPath