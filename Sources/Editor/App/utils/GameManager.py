class _GameManager:

    _INSTANCE = None

    def __init__(self, app):
        self._app = app

    def startGame(self):
        gameMonitor = self._app._editorNative.getGameMonitor()
        gameMonitor.startGame()

def GetGameManager():
    return _GameManager._INSTANCE

def CreateGameManager(app):
    manger = _GameManager(app)
    _GameManager._INSTANCE = manger