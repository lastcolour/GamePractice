class CopyEntityData:
    def __init__(self):
        self._name = None
        self._isInternal = False
        self._data = None

class CopyPasteManager:

    def __init__(self, mgr):
        self._mgr = mgr
        self._copyLogicData = None
        self._copyEntityData = None
        self._copyFileName = None
        self._doCutForFile = False

    def setCopyFile(self, fileName, doCut):
        self._copyFileName = fileName
        self._doCutForFile = doCut

    def getCopyFile(self):
        return self._copyFileName

    def getCutFlag(self):
        return self._doCutForFile

    def getCopyEntity(self):
        return self._copyEntityData

    def setCopyEntity(self, entity):
        self._copyEntityData = CopyEntityData()
        self._copyEntityData._name = entity.getName()
        if entity.isInternal() or entity.getParent() == None:
            self._copyEntityData._isInternal = True
            self._copyEntityData._data = entity.dumpToDict()
        else:
            self._copyEntityData._isInternal = False

    def setCopyLogic(self, logic):
        self._copyLogicData = {}
        logic.writeToDict(self._copyLogicData)

    def getCopyLogic(self):
        return self._copyLogicData