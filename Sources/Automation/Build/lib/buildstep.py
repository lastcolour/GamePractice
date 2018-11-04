from .cmake import CmakeRunner
from .compile import CompileRunner
from .install import Installer

from .logger import log

__all__ =  ["BuildStep"]

class BuildStep:
    def __init__(self, config):
        self._stepName = config.value("step-name")
        self._cmake = CmakeRunner(config)
        self._compiler = CompileRunner(config)
        self._intaller = Installer(config)

    def run(self):
        log.info("[Info] Start build step: '{0}'".format(self._stepName))
        self._cmake.run()
        self._compiler.run()
        self._intaller.run()

    def getOutDirs(self):
        return []