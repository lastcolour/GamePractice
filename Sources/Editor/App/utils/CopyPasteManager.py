class _CopyPasteManager:

    _INSTANCE = None

    def __init__(self, app):
        self._app = app
        self._copyLogicData = None

    def setCopyLogic(self, logic):
        self._copyLogicData = {}
        logic.writeToDict(self._copyLogicData)

    def getCopyLogic(self):
        return self._copyLogicData

def GetCopyPasteManager():
    return _CopyPasteManager._INSTANCE

def CreateCopyPasteManager(app):
    manger = _CopyPasteManager(app)
    _CopyPasteManager._INSTANCE = manger