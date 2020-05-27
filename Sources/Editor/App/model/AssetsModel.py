import os
import pathlib

class FileNodeType:
    Entity = 0
    Dir = 0

class FileNode:
    def __init__(self):
        self._parent = None
        self._name = None

    def isDir(self):
        return False

    def getBaseName(self):
        return self._name

    def getRelativePath(self):
        resPath = self._name
        currParent = self._parent
        while currParent._parent is not None:
            resPath = "{0}/{1}".format(self._parent._name, resPath)
            currParent = currParent._parent
        return resPath

    def getFullPath(self):
        resPath = self._name
        currParent = self._parent
        while currParent is not None:
            resPath = "{0}/{1}".format(self._parent._name, resPath)
            currParent = currParent._parent
        return resPath

class DirNode(FileNode):
    def __init__(self):
        super().__init__()
        self._children = []

    def isDir(self):
        return True

    def getChildren(self):
        return self._children

    def __iter__(self):
        for child in self._children:
            yield child

class AssetsModel:
    def __init__(self, appConfig):
        self._appConfig = appConfig
        self._entitiesRootDir = DirNode()

    def _scanDir(self, dirNode):
        dirPath = dirNode.getFullPath()
        if not os.path.exists(dirPath):
            raise RuntimeError("Invalid scan dir: {0}".format(dirPath))
        for item in os.listdir(dirPath):
            itemPath = "{0}/{1}".format(dirPath, item)
            if os.path.isdir(itemPath):
                node = DirNode()
                node._name = item
                node._parent = dirNode
                self._scanDir(node)
                dirNode._children.append(node)
            else:
                node = FileNode()
                node._name = item
                node._parent = dirNode
                dirNode._children.append(node)

    def init(self):
        assetsRootDir = self._appConfig.getAssetsRootPath()
        if not os.path.exists(assetsRootDir):
            print("[AssetsModel:init] Can't find assets root dir: '{0}'".format(assetsRootDir))
            return False
        self._entitiesRootDir = DirNode()
        self._entitiesRootDir._name = "{0}/Entities".format(assetsRootDir)
        self._scanDir(self._entitiesRootDir)
        return True

    def getEntitiesTree(self):
        return self._entitiesRootDir