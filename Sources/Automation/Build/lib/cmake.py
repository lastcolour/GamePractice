import os
import shutil

from .runner import Runner
from .logger import log

class CmakeRunner(Runner):

    def __init__(self, configs):
        Runner.__init__(self)
        self._cmakeRunDir = None
        self._cmakeOutDir = None
        self._cmakeLogFile = None
        self._generator = None
        self._definitions = []
        self._cmakePath = None
        self._buildType = configs.value("build-type")
        self._cleanBuild = configs.value("force-clean-build")
        self._setUp(configs.get("cmake"))

    def run(self):
        log.info("[Info] Start cmake project generation")
        tCmd = self._formatCmd()
        tLogFileName = None
        if self._cmakeLogFile is not None:
            tLogFileName = self._cmakeOutDir + '/' + self._cmakeLogFile
        tCMDData = self._runCMD(tCmd, workDir=self._cmakeRunDir, logFileName=tLogFileName)
        if tCMDData["ret_code"] is 0:
            log.info("[Info] Project generated successfully!")
        else:
            log.error("[Error] Fail to generate cmake project")
            if "out" in tCMDData:
                self._printNiceLog(tCMDData["out"])
            else:
                log.warning("[Warning] No any error's log provided!")
            raise RuntimeError("Can't generate cmake project")

    def _getBuildType(self):
        if self._buildType is None:
            log.warning("[Warning] Cmake build type not provider")
            return ""
        return " -DCMAKE_BUILD_TYPE={0}".format(self._buildType)

    def _getDefs(self):
        if self._definitions is None or len(self._definitions) is 0:
            return ""
        tDefs = ""
        for item in self._definitions:
            tDefs += " -D{0}".format(item)
        return tDefs

    def _getGenerator(self):
        if self._generator is None:
            return ""
        return " -G\"{0}\"".format(self._generator)

    def _formatCmd(self):
        return "{0}cmake -H. -B{1}".format(self._cmakePath, self._cmakeOutDir) + self._getBuildType() + \
                    self._getDefs() + self._getGenerator()

    def _setUpDefs(self, configs):
        if "defs" not in configs:
            return
        tDefsList = []
        tDefsConfig = configs.get("defs")
        if "All" in tDefsConfig:
            for item in tDefsConfig.get("All"):
                tDefsList.append(item)
        if self._buildType in tDefsConfig:
            for item in tDefsConfig.get(self._buildType):
                tDefsList.append(item)
        if configs.platform() in tDefsConfig:
            for item in tDefsConfig.get(configs.platform()):
                tDefsList.append(item)
        for item in tDefsList:
            self._definitions.append("{0}={1}".format(item.getTreeName(), item.value()))

    def _setUpDirs(self, configs):
        self._cmakeRunDir = configs.value("run-dir")
        if not os.path.exists(self._cmakeRunDir):
            log.error("[Error] Can't find cmake run dir: {0}".format(self._cmakeRunDir))
            raise ValueError("Invalid cmake configs")
        self._cmakeOutDir = configs.value("out-dir")
        if os.path.exists(self._cmakeOutDir) and self._cleanBuild:
            try:
                log.info("[Info] Remove old cmake build directory: {0}".format(self._cmakeOutDir))
                shutil.rmtree(self._cmakeOutDir)
            except:
                log.error("[Error] Can't clear build directory: {0}".format(self._cmakeOutDir))
                raise
        if not os.path.exists(self._cmakeOutDir):
            try:
                os.makedirs(self._cmakeOutDir)
            except:
                log.error("[Error] Can't create cmake out dir: {0}".format(self._cmakeOutDir))
                raise

    def _setUp(self, configs):
        self._setUpDefs(configs)
        self._setUpDirs(configs)
        self._cmakeLogFile = configs.value("log-file")
        if self._cmakeLogFile is None:
            log.warning("[Warning] Cmake log file not specified!")
        self._generator = configs.platformValue("generator")
        if self._generator is None:
            log.warning("[Warning] Cmake generator not specified")
        self._cmakePath = configs.platformValue("cmake-bin-path")
        if len(self._cmakePath) > 0:
            self._cmakePath = self._cmakePath + "/"

    def getOutDir(self):
        return self._cmakeOutDir