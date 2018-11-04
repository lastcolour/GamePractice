import sys

from .runner import Runner
from .logger import log

class CompileRunner(Runner):

    def __init__(self, config):
        Runner.__init__(self)
        self._compiler = None
        self._compileArgs = ""
        self._compileRunDir = config.get("cmake").value("out-dir")
        self._compileLogFile = config.get("compile").value("log-file")
        self._setUp(config.get("compile"))

    def run(self):
        log.info("[Info] Start compile project")
        tCmd = self._formatCmd()
        tLogFileName = None
        if self._compileLogFile is not None:
            tLogFileName = self._compileRunDir + '/' + self._compileLogFile
        tCMDData = self._runCMD(tCmd, workDir=self._compileRunDir, logFileName=tLogFileName)
        if tCMDData["ret_code"] is 0:
            log.info("[Info] Project compiled successfully!")
        else:
            log.error("[Error] Fail to compile project")
            if "out" in tCMDData:
                self._printNiceLog(tCMDData["out"], tLogFileName)
            else:
                log.warning("[Warning] No any error's log provided!")
            raise RuntimeError("Compilation failed")

    def _getCompiler(self, configs):
        tCompiler = configs.platformValue("compiler")
        if tCompiler is None:
            raise RuntimeError("Can't determine compiler for such platform")
        return tCompiler

    def _setUp(self, configs):
        self._compiler = self._getCompiler(configs)
        if self._compiler is None:
            raise RuntimeError("Invalid compiler")
        tNodeArgsName = "compiler-args"
        if tNodeArgsName not in configs:
            log.debug("[Debug] No additional config provided for '{0}'".format(self._compiler))
            return
        tCompArgsNode = configs.get(tNodeArgsName)
        if self._compiler not in tCompArgsNode:
            return
        tArgsList = []
        for item in tCompArgsNode.get(self._compiler):
            tArgsList.append(item.value())
        self._compileArgs = " ".join(tArgsList)

    def _formatCmd(self):
        if len(self._compileArgs) == " ":
            return self._compiler
        else:
            return "{0} {1}".format(self._compiler, self._compileArgs)