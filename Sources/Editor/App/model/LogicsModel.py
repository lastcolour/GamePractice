class ModuleNode:
    def __init__(self):
        self._name = None
        self._logics = []

    def getName(self):
        return self._name

    def getLogics(self):
        return self._logics

class LogicNode:
    def __init__(self):
        self._name = None

    def getName(self):
        return self._name

class LogicsModel:
    def __init__(self, editorNative):
        self._editorNative = editorNative
        self._moduleLogics = []

    def init(self):
        moduleLogics = self._editorNative.getReflectModel().getAllRegisteredLogics()
        for moduleName in moduleLogics:
            moduleNode = ModuleNode()
            moduleNode._name = moduleName
            for logicName in moduleLogics[moduleName]:
                logicNode = LogicNode()
                logicNode._name = logicName
                moduleNode._logics.append(logicNode)
            self._moduleLogics.append(moduleNode)
        self._moduleLogics.sort(key=lambda x : x._name, reverse=False)
        for module in self._moduleLogics:
            module._logics.sort(key=lambda x : x._name, reverse=False)
        return True

    def getLogics(self):
        return self._moduleLogics