import os
import sys
import time

from subprocess import Popen

from .logger import log

_TMP_FILE = "data.temp"

class Runner:

    def __init__(self):
        pass

    def __isNeedUseShell(self):
        return sys.platform.startswith("linux")

    def __createPipe(self, logFileName):
        if type(logFileName) == type(sys.stdout):
            return logFileName
        if logFileName is None:
            return sys.stdout
        try:
            return open(logFileName, 'w+')
        except:
            log.error("[Error] Can't create file for process log: {0}".format(logFileName))
            return None

    def _runCMD(self, cmd, workDir=None, logFileName=None):
        self._checkWorkDir(workDir)
        tPipe = self.__createPipe(logFileName)
        tStartPoint = time.time()
        tProcData = dict()
        iSUsedTmpFile = False
        if tPipe is None:
            pipeFile = open(_TMP_FILE, "w+")
            iSUsedTmpFile = True
        else:
            pipeFile = tPipe
        try:
            log.debug("[Debug] Start process: {0}".format(cmd if type(cmd) != list else " ".join(cmd)))
            log.debug("[Debug] Working directory: {0}".format(workDir))
            tProc = Popen(cmd, cwd=workDir, shell=self.__isNeedUseShell(), stdout=pipeFile, stderr=pipeFile)
            tProc.wait()
        except:
            log.error("[Error] Can't run process: {0}".format(cmd))
            log.error("[Error] Problem: {0}".format(sys.exc_info()[1]))
            tProcData["ret_code"] = None
        else:
            tProcData["ret_code"] = tProc.returncode
        finally:
            if pipeFile != sys.stdout:
                pipeFile.tell()
                pipeFile.seek(0)
                tProcData["out"] = pipeFile.read()
                pipeFile.close()
                if iSUsedTmpFile:
                    os.remove(_TMP_FILE)
            log.debug("[Debug] Process execution duration: {0:.1f}s".format(time.time() - tStartPoint))
            return tProcData

    def _checkWorkDir(self, dirPath):
        if not os.path.exists(dirPath):
            raise RuntimeError("Can't find run dir")

    def _printNiceLog(self, message, logFile=None):
        if logFile is not None:
            log.warning("[Warning] Fail log was saved to {0}".format(logFile))
        log.warning("[Warning] -------- Process out -------")
        tLineDecoration = "|"
        for line in message.split("\n"):
           log.warning("{0} {1}".format(tLineDecoration, line))
        log.warning("[Warning] ----------------------------")