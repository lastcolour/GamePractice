class GameManager:

    def __init__(self, mgr):
        self._mgr = mgr

    def startGame(self):
        gameMonitor = self._mgr._app._editorNative.getGameMonitor()
        gameMonitor.startGame()