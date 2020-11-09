class CopyEntityData:
    def __init__(self):
        self._name = None
        self._data = None

class CopyPasteManager:

    def __init__(self, mgr):
        self._mgr = mgr
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