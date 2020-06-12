class MouseEventType:
    Press = 0
    Move = 1
    Release = 2

class _EngineViewManager:

    _INSTANACE = None

    def __init__(self, app):
        self._app = app

    def drawNativeFrameTo(self, ptr, width, height):
        self._app._editorNative.getLibrary().drawFrame(ptr, width, height)

    def onOutputFrameSizeChanged(self, width, height):
        self._app.statusBar().onOutputFrameSizeChanged(width, height)

    def onEnableGameUpdate(self, flag):
        pass

    def onTimeScaleChanged(self, timeScale):
        pass

    def onOrientationChanged(self, isHorizontal):
        self._app._engineOutputView._engineOutFrame.setOrientation(isHorizontal)

    def onAspectRatioChanged(self, aspectRation):
        self._app._engineOutputView._engineOutFrame.setAspectRatio(aspectRation)

    def onMouseInputEvent(self, eventType, x, y):
        self._app._editorNative.getLibrary().mouseInputEvent(eventType, x, y)

def GetEngineViewManager():
    return _EngineViewManager._INSTANCE

def CreateEngineViewManager(app):
    manger = _EngineViewManager(app)
    _EngineViewManager._INSTANCE = manger