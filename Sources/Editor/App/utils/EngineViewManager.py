class MouseEventType:
    Press = 0
    Move = 1
    Release = 2

class EngineViewManager:

    def __init__(self, mgr):
        self._mgr = mgr
        self._app = self._mgr._app

    def drawNativeFrameTo(self, ptr, width, height):
        self._app._editorNative.getLibrary().drawFrame(ptr, width, height)

    def onOutputFrameSizeChanged(self, width, height):
        self._app.statusBar().onOutputFrameSizeChanged(width, height)

    def onEnableGameUpdate(self, flag):
        self._app._editorNative.getLibrary().enableGameUpdate(flag)

    def onTimeScaleChanged(self, timeScale):
        self._app._editorNative.getLibrary().setGameTimeScale(timeScale)

    def onOrientationChanged(self, isHorizontal):
        self._app._engineOutputView._engineOutFrame.setOrientation(isHorizontal)

    def onAspectRatioChanged(self, aspectRation):
        self._app._engineOutputView._engineOutFrame.setAspectRatio(aspectRation)

    def onMouseInputEvent(self, eventType, x, y):
        self._app._editorNative.getLibrary().mouseInputEvent(eventType, x, y)