import subprocess
import pathlib
import os

class GameMonitor:
    def __init__(self, appConfig):
        self._gamePath = appConfig._gameBinPath

    def startGame(self):
        if not os.path.exists(self._gamePath):
            print("[GameMonitor:startGame] Can't find game executable: '{0}'".format(self._gamePath))
            return
        cwdDir = pathlib.Path(self._gamePath).parent
        try:
            print("[GameMonitor:startGame] Start process: '{0}'".format(self._gamePath))
            subprocess.Popen([self._gamePath,], cwd=cwdDir.__str__(), stderr=subprocess.STDOUT)
        except Exception as e:
            print("[GameMonitor:startGame] Can't start process: {0}".format(self._gamePath))
            pass