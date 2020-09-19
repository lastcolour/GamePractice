class CopyEntityData:
    def __init__(self):
        self._name = None
        self._data = None

class _CopyPasteManager:

    _INSTANCE = None

    def __init__(self, app):
        self._app = app
        self._copyLogicData = None
        self._copyEntityData = None

    def getCopyEntity(self):
        return self._copyEntityData

    def setCopyEntity(self, entity):
        self._copyEntityData = CopyEntityData()
        self._copyEntityData._data = entity.dumpToDict()
        self._copyEntityData._name = entity.getName()

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