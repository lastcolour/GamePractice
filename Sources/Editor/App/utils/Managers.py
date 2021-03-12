from utils.EventManager import EventManager
from utils.EngineViewManager import EngineViewManager
from utils.MainViewManager import MainViewManager
from utils.CopyPasteManager import CopyPasteManager
from utils.GameManager import GameManager
from msg.MessageSystem import MessageSystem

class _Managers:

    _INSTANCE = None

    def __init__(self, app):
        self._app = app
        self._msgSystem = MessageSystem()
        self._eventMgr = EventManager(self)
        self._engineViewMgr = EngineViewManager(self)
        self._mainViewMgr = MainViewManager(self)
        self._copyPasteMgr = CopyPasteManager(self)
        self._gameMgr = GameManager(self)

def CreateManagers(app):
    _Managers._INSTANCE = _Managers(app)

def GetCopyPasteManager():
    return _Managers._INSTANCE._copyPasteMgr

def GetGameManager():
    return _Managers._INSTANCE._gameMgr

def GetEventManager():
    return _Managers._INSTANCE._eventMgr

def GetEngineViewManager():
    return _Managers._INSTANCE._engineViewMgr

def GetMainViewManager():
    return _Managers._INSTANCE._mainViewMgr
