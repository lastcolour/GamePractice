import os
import shutil

from utils.Log import Log
from native.EntityNativeLoader import CreateVoidEntity

class FileNodeType:
    Invalid = 0
    Entity = 1
    Image = 2
    Dir = 3
    Sound = 4
    VertShader = 5
    FragShader = 6
    TTFFont= 7

def _getFileType(dirPath, baseName):
    tokens = baseName.split(".")
    if len(tokens) < 2:
        return FileNodeType.Invalid
    lastToken = tokens[-1]
    if lastToken == "png":
        return FileNodeType.Image
    elif lastToken == "ogg":
        return FileNodeType.Sound
    elif lastToken == "json":
        return FileNodeType.Entity
    elif lastToken == "frag":
        return FileNodeType.FragShader
    elif lastToken == "vert":
        return FileNodeType.VertShader
    elif lastToken == "ttf":
        return FileNodeType.TTFFont
    else:
        Log.warning("[AssetsModel:_getFileType] Can't determine file type: '{0}/{1}'".format(
            dirPath, baseName))
        return FileNodeType.Invalid

class FileNode:
    def __init__(self, parent, name):
        self._parent = parent
        self._name = name
        self._type = None

    def isDir(self):
        return False

    def isRoot(self):
        if self._parent is None:
            raise RuntimeError("Invalid node")
        else:
            return self._parent._parent is None

    def getBaseName(self):
        return self._name

    def getRelativePath(self):
        resPath = self._name
        currParent = self._parent
        while currParent._parent is not None:
            resPath = "{0}/{1}".format(currParent._name, resPath)
            currParent = currParent._parent
        return resPath

    def getFullPath(self):
        resPath = self._name
        currParent = self._parent
        while currParent is not None:
            resPath = "{0}/{1}".format(currParent._name, resPath)
            currParent = currParent._parent
        return resPath

    def getParentFullPath(self):
        currParent = self._parent._parent
        resPath = self._parent._name
        while currParent is not None:
            resPath = "{0}/{1}".format(currParent._name, resPath)
            currParent = currParent._parent
        return resPath

class DirNode(FileNode):
    def __init__(self, parent, name):
        super().__init__(parent, name)
        self._type = FileNodeType.Dir
        self._children = []

    def isDir(self):
        return True

    def getChildren(self):
        return self._children

    def __iter__(self):
        for child in self._children:
            yield child

    def findChild(self, name):
        for child in self._children:
            if child._name == name:
                return child
        return None

class AssetsModel:
    def __init__(self, appConfig):
        self._appConfig = appConfig
        self._resourceRootDir = DirNode(None, None)

    def _scanDir(self, dirNode):
        dirPath = dirNode.getFullPath()
        if not os.path.exists(dirPath):
            raise RuntimeError("Invalid scan dir: '{0}'".format(dirPath))
        for item in os.listdir(dirPath):
            itemPath = "{0}/{1}".format(dirPath, item)
            if os.path.isdir(itemPath):
                node = DirNode(dirNode, item)
                self._scanDir(node)
                dirNode._children.append(node)
            else:
                node = FileNode(dirNode, item)
                node._type = _getFileType(dirPath, item)
                dirNode._children.append(node)
        dirNode._children.sort(key=lambda node: node._name)
        dirNode._children.sort(key=lambda node: not node.isDir())

    def createNewDir(self, parentNode, fileName):
        if len(fileName) == 0:
            Log.error("[AssetsModel:createNewDir] Can't create with empty name")
            return None
        dirPath = "{0}/{1}".format(parentNode.getFullPath(), fileName)
        if os.path.exists(dirPath):
            Log.error("[AssetsModel:createNewDir] Can't create dir: '{0}' (Error: already exists)".format(dirPath))
            return None
        try:
            os.mkdir(dirPath)
        except Exception as e:
            Log.error("[AssetsModel:createNewDir] Can't create dir: '{0}' (Error: {1})".format(dirPath, e.__str__()))
            return None
        Log.debug("[AssetsModel:createNewDir] Create dir: '{0}'".format(dirPath))
        node = DirNode()
        node._name = fileName
        node._parent = parentNode
        parentNode._children.append(node)
        return node

    def createNewEntity(self, parentNode, fileName):
        if len(fileName) == 0:
            Log.error("[AssetsModel:createNewEntity] Can't create with empty name")
            return None
        filePath = "{0}/{1}".format(parentNode.getFullPath(), fileName)
        if os.path.exists(filePath):
            Log.error("[AssetsModel:createNewEntity] Can't create file: '{0}' (Error: already exists)".format(filePath))
            return None
        try:
            CreateVoidEntity(filePath)
        except Exception as e:
            Log.error("[AssetsModel:createNewEntity] Can't create file: '{0}' (Error: {1})".format(filePath, e.__str__()))
            return None
        Log.debug("[AssetsModel:createNewEntity] Create file: '{0}'".format(filePath))
        node = FileNode()
        node._name = fileName
        node._parent = parentNode
        node._type = FileNodeType.Entity
        parentNode._children.append(node)
        return node

    def removeNode(self, node):
        filePath = node.getFullPath()
        if os.path.exists(filePath):
            try:
                if os.path.isdir(filePath):
                    Log.debug("[AssetsModel:removeNode] Remove dir: '{0}'".format(filePath))
                    shutil.rmtree(filePath)
                else:
                    Log.debug("[AssetsModel:removeNode] Remove file: '{0}'".format(filePath))
                    os.remove(filePath)
            except Exception as e:
                Log.error("[AssetsModel:removeNode] Can't remove file: '{0}' (Error: {1})".format(filePath, e.__str__()))
                return False
        node._parent._children.remove(node)
        return True

    def renameNode(self, node, newName):
        srcPath = node.getFullPath()
        trgPath = "{0}/{1}".format(node.getParentFullPath(), newName)
        if os.path.exists(trgPath):
            Log.error("[AssetsModel:renameNode] Target path already exists: '{0}'".format(trgPath))
            return False
        if not os.path.exists(srcPath):
            Log.error("[AssetsModel:renameNode] Can't find source path: '{0}'".format(srcPath))
            return False
        try:
            os.replace(srcPath, trgPath)
        except Exception as e:
            Log.error("[AssetsModel:renameNode] Can't rename file: '{0}' to '{1}' (Error: {2})".format(
                srcPath, trgPath, e.__str__()))
            return False
        Log.debug("[AssetsModel:renameNode] Rename file: '{0}' to '{1}'".format(srcPath, trgPath))
        node._name = newName
        return True

    def init(self):
        assetsRootDir = self._appConfig.getAssetsRootPath()
        if not os.path.exists(assetsRootDir):
            Log.error("[AssetsModel:init] Can't find assets root dir: '{0}'".format(assetsRootDir))
            return False
        self._resourceRootDir = DirNode(None, assetsRootDir)
        self._scanDir(self._resourceRootDir)
        return True

    def reload(self):
        Log.debug("[AssetsModel:reload] Trigger Reload")
        self._resourceRootDir = DirNode(None, None)
        if not self.init():
            raise RuntimeError("Can't reload assets model")

    def getEntitiesTree(self):
        entRoot = self._resourceRootDir.findChild("Entities")
        return entRoot

    def getResourcesTree(self):
        return self._resourceRootDir