import os
import pathlib
import platform

class AppConfig:
    def __init__(self):
        self._buildType = "RelWithDebInfo"
        self._platform = platform.system()
        self._projectRootPath = self._getProjectRootPath()
        self._assetsRootPath = "{0}/{1}".format(self._projectRootPath, "Assets")
        self._nativeLibName = "Editor"
        self._nativeLibPath = self._getNativeLibPath()
        self._gameBinName = "Game01"
        self._gameBinPath = self._getGameBinPath()

    def getNativeLibPath(self):
        return self._nativeLibPath

    def getAssetsRootPath(self):
        return self._assetsRootPath

    def _getNativeLibName(self):
        if self._platform == "Linux":
            return "lib{0}.so".format(self._nativeLibName)
        elif self._platform == "Windows":
            return "{0}.dll".format(self._nativeLibName)
        raise RuntimeError("Unsuported platform")

    def _getNativeGameBinName(self):
        if self._platform == "Linux":
            return "{0}".format(self._gameBinName)
        elif self._platform == "Windows":
            return "{0}.exe".format(self._gameBinName)
        raise RuntimeError("Unsuported platform")

    def _getGameBinPath(self):
        return self._getBuildArtifactPath(self._getNativeGameBinName())

    def _getBuildArtifactPath(self, buildArtifact):
        buildDir = "{0}/_build".format(self._projectRootPath)
        cPath =  pathlib.Path("{0}/{1}/Game/{2}/{3}".format(
            buildDir, self._platform, self._buildType, buildArtifact)).__str__()
        resPath = cPath.replace("\\", "/")
        if not os.path.exists(resPath):
            raise RuntimeError("Can't find native lib path: '{0}'".format(resPath))
        return resPath

    def _getNativeLibPath(self):
        return self._getBuildArtifactPath(self._getNativeLibName())

    def _getProjectRootPath(self):
        cPath = pathlib.Path("{0}/../../../../../..".format(__file__)).parent
        resPath = cPath.resolve().__str__()
        resPath = resPath.replace("\\", "/")
        if not os.path.exists(resPath):
            raise RuntimeError("Can't find project root path: '{0}'".format(resPath))
        return resPath