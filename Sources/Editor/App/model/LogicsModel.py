class LogicNode:
    def __init__(self):
        self._name = None

    def getName(self):
        return self._name

class LogicsModel:
    def __init__(self, editorNative):
        self._editorNative = editorNative
        self._logics = []

    def init(self):
        logics = self._editorNative.getReflectModel().getAllRegisteredLogics()
        for item in logics:
            node = LogicNode()
            node._name = item
            self._logics.append(node)
        return True

    def getLogics(self):
        return self._logics