from .runner import Runner
from .logger import log

class TestRunner(Runner):
    def __init__(self, config):
        Runner.__init__(self)
        self._testApp = config.value("name")
        self._runDir = config.value("run-dir")

    def run(self):
        log.info("[Info] Run unittest: {0}".format(self._testApp))
        tCMDData = self._runCMD(self._testApp, workDir=self._runDir)
        if tCMDData["ret_code"] != 0:
            log.warning("[Warning] Unittests have failed")
