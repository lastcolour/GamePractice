class DirNode:
    def __init__(self):
        self._name = None
        self._children = []

class FileNode:
    def __init__(self):
        self._name = None

class AssetsModel:
    def __init__(self):
        self._entitiesRootDir = DirNode()

    def init(self):
        return True

    def getEntitiesTree(self):
        pass