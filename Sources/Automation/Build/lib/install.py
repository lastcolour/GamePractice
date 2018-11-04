import shutil
import glob
import os

from .logger import log

class Installer:
    def __init__(self, configs):
        self._prjName = configs.getTreeName()
        self._configs = None
        if "install" in configs:
            self._configs = configs.get("install")

    def run(self):
        if self._configs is None:
            return
        log.info("[Info] Installing: {0}".format(self._prjName))
        for group in self._configs:
            self._installGroup(group)

    def _installGroup(self, group):
        if "files" in group and not "dirs" in group:
            self._installFiles(group)
        elif "dir" in group and not "files" in group:
            self._installDir(group)
        else:
            raise RuntimeError("Can't install group")

    def _installDir(self, group):
        tDirName = group.value("dir")
        tFromPath = group.value("from")
        tToPath = group.value("to")
        tSrcDirPath = tFromPath + "/" + tDirName
        tDstDirPath = tToPath + "/" + tDirName
        if os.path.exists(tDstDirPath) and group.value("force"):
            log.debug("[Debug] Remove old dir: {0}".format(tDstDirPath))
            shutil.rmtree(tDstDirPath)
        elif os.path.exists(tDstDirPath):
            log.warning("[Warning] Don't copy dir: {0} (Already exist)".format(tDirName))
            return
        log.debug("[Debug] Copy dir: {0}".format(tDirName))
        shutil.copytree(tSrcDirPath, tDstDirPath)

    def _installFiles(self, group):
        tFromPath = group.value("from")
        tToPath = group.value("to")
        if not os.path.exists(tToPath):
            os.makedirs(tToPath)
        for maskItem in group.getRaw("files"):
            pathSplicer = "/"
            if tFromPath[-1] == "/" or tFromPath[-1] == "\\":
                pathSplicer = ""
            isForce = group.value("force")
            isInstall = False
            for item in glob.iglob(tFromPath + pathSplicer + maskItem):
                isInstall = True
                tBaseName = os.path.basename(item)
                tResPath = tToPath + "/" + tBaseName
                if not os.path.exists(tResPath) or isForce:
                    log.debug("[Debug] Copy file: {0}".format(os.path.basename(tBaseName)))
                    shutil.copy(item, tResPath)
                else:
                    log.debug("[Debug] Skipping: {0} (already exist)".format(tBaseName))
            if not isInstall:
                log.warning("[Warning] Can't install any file by mask: {0}".format(maskItem))