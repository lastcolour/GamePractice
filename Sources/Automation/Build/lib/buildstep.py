from .cmake import CmakeRunner
from .compile import CompileRunner
from .install import Installer

from .logger import log

__all__ =  ["BuildStep"]

class BuildStep:
    def __init__(self, config):
        self._stepName = config.value("step-name")
        self._tags = config.value("tags")
        self._cmake = CmakeRunner(config)
        self._compiler = CompileRunner(config)
        self._installer = Installer(config)

    def run(self):
        log.info("[Info] Start build step: '{0}'".format(self._stepName))
        self._cmake.run()
        self._compiler.run()
        self._installer.run()

    def getName(self):
        return self._stepName

    def hasTag(self, filterTag):
        return filterTag in self._tags

    def getOutDirs(self):
        return []