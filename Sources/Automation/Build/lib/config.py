import os
import copy
import json
from collections import OrderedDict

from string import Formatter

from .logger import log

__all__ = ["ConfigNode"]

_LOCAL_CONFIGS_EXAMPLE = {
    "android-sdk" : None,
    "android-cmake-path" : None
}

def _isStrType(some_type):
    try:
        return eval("some_type is str or some_type is unicode")
    except NameError:
        return some_type is str

def _isDictType(some_type):
    return some_type is dict or some_type is type(OrderedDict())

class ConfigNode:

    _ARGS_VARS = {}
    _CONFIG_VARS = {}

    @staticmethod
    def setArgVars(buildType, platform):
        ConfigNode._ARGS_VARS["build-type"] = buildType
        ConfigNode._ARGS_VARS["platform"] = platform

    def __init__(self, parent=None, tree=None, treeName=None):
        self._parent = parent
        self._treeName = treeName
        self._tree = tree
        self._fmtVars = {}
        self._inheritParentFmtVars()

    def __iter__(self):
        isList = type(self._tree) is list
        for item in self._tree:
            if isList:
                yield ConfigNode(self, item, "List Element")
            else:
                yield ConfigNode(self, self._tree[item], item)

    def __contains__(self, item):
        return item in self._tree

    def isLinux(self):
        return ConfigNode._ARGS_VARS["platform"].startswith("Lin")

    def isWindows(self):
        return ConfigNode._ARGS_VARS["platform"].startswith("Win")

    def isAndroid(self):
        return ConfigNode._ARGS_VARS["platform"].startswith("And")

    def platform(self):
        return ConfigNode._ARGS_VARS["platform"]

    def value(self, configKey=None):
        if _isDictType(type(self._tree)) and configKey is None:
            raise RuntimeError("Invalid operation under tree-like config")
        elif configKey is None:
            return self._formatValue(self._tree)
        if configKey not in self._tree:
            log.error("[Error] Can't find config key: '{0}' in config tree: '{1}'".format(configKey,
                                                                                          self.getTreeName()))
            raise RuntimeError("Unknown config key")
        tValue = self._tree[configKey]
        if not _isStrType(type(tValue)):
            return tValue
        return self._formatValue(tValue)

    def platformValue(self, configKey):
        if configKey not in self._tree:
            log.error("[Error] Can't find config key: '{0}' in config tree: '{1}'".format(configKey,
                                                                                          self.getTreeName()))
            raise RuntimeError("Unknown config key")
        tNode = self._tree[configKey]
        tPlatform = ConfigNode._ARGS_VARS["platform"]
        if tPlatform not in tNode:
            log.error("[Error] Can't find platform config key '{0}' in config tree {1}".format(configKey,
                                                                                               self.getTreeName()))
            raise RuntimeError("Unknown platform config key")
        return self._formatValue(tNode[tPlatform])

    def get(self, configKey):
        if configKey not in self._tree:
            log.error("[Error] Can't find config key: '{0}' in config tree: '{1}'".format(configKey,
                                                                                          self.getTreeName()))
            raise RuntimeError("Unknown config key")
        return ConfigNode(self, self._tree[configKey], configKey)

    def getRaw(self, configKey):
        if configKey not in self._tree:
            log.error("[Error] Can't find config key: '{0}' in config tree: '{1}'".format(configKey,
                                                                                          self.getTreeName()))
            raise RuntimeError("Unknown config key")
        return self._tree[configKey]

    def load(self, configFile):
        if self._tree is not None:
            log.error("[Warning] Reloading tree: '{0}' from file: {1}".format(self.getTreeName(), configFile))
        try:
            with open(configFile, "r") as tFile:
                self._tree = json.load(tFile, object_pairs_hook=OrderedDict)
            self._treeName = "Source: {0}".format(configFile)
            self._fmtVars["this-file-path"] = os.path.dirname(os.path.realpath(configFile))
        except:
            log.error("[Error] Can't load config from file: {0}".format(configFile))
            raise
        else:
            log.info("[Info] Loaded config file: {0}".format(configFile))

    def set(self, configKey, value):
        if configKey in self._tree:
            log.warning("[Warning] Trying override config value: {0}; {1} -> {2}".format(
                configKey, self._tree[configKey], value))
        self._tree[configKey] = value

    def createFmtVars(self, configKey):
        if configKey not in self._tree:
            log.error("[Error] Can't find format vars config node: {0}".format(configKey))
            raise RuntimeError("Can't create format args")
        tFmtDict = self._tree[configKey]
        for fmtKey in tFmtDict:
            self._constructFmtVar({fmtKey : tFmtDict[fmtKey]})

    def extendFromFile(self, filename):
        tRealFilePath = os.path.abspath(filename)
        try:
            with open(tRealFilePath, "r") as tFile:
                tTree = json.load(tFile, object_pairs_hook=OrderedDict)
        except:
            log.error("[Error] Can't load extension config: '{0}'".format(tRealFilePath))
            raise
        else:
            self._extendTree(self._tree, tTree)

    def _extendTree(self, targetTree, extTree):
        for keyName in extTree:
            tExtType = type(extTree[keyName])
            if tExtType is type(None):
                if keyName not in targetTree:
                    log.error("[Error] Can't extend tree. Key require value in basic tree: {0}".format(keyName))
                    raise RuntimeError("Can't extend config node")
            elif _isDictType(tExtType):
                if keyName not in targetTree:
                    tTree = {}
                    self._extendTree(tTree, extTree[keyName])
                    targetTree[keyName] = tTree
                else:
                    self._extendTree(targetTree[keyName], extTree[keyName])
            elif tExtType is list:
                if keyName not in targetTree:
                    targetTree[keyName] = extTree[keyName]
            else:
                if keyName not in targetTree:
                    targetTree[keyName] = extTree[keyName]
                else:
                    if tExtType != type(targetTree[keyName]):
                        log.warning("[Warning] Extension and target trees has diff types for value: {0}: ({1} <-> {2})"
                                    .format(keyName, tExtType, type(targetTree[keyName])))

    def _inheritParentFmtVars(self):
        if self._parent is not None:
            self._fmtVars = self._parent._fmtVars
        else:
            self._fmtVars = {}

    def _constructFmtVar(self, fmtVarNode):
        if not _isDictType(type(fmtVarNode)):
            log.error("[Error] Can't create format var from: {0}, type={1} (Invalid node type, Should by dict"
                      .format(fmtVarNode, type(fmtVarNode)))
            raise RuntimeError("Can't construct format var")
        if len(fmtVarNode) != 1:
            log.error("[Error] Can't create format var from: {0} (Invalid syntax, Ex: 'var-name:var-value')"
                      .format(fmtVarNode))
            raise RuntimeError("Can't construct format var")
        tFmtValName = list(fmtVarNode)[0]
        tFmtResValName = tFmtValName
        tFmtAdditionalNameFmt = tFmtValName.split(":")
        tFmtStyle = None
        if len(tFmtAdditionalNameFmt) > 1:
            if len(tFmtAdditionalNameFmt) != 2:
                log.error("[Error] Can't apply additional format to variable name: {0}".format(tFmtValName))
                raise RuntimeError("Can't construct format var")
            tFmtResValName = tFmtAdditionalNameFmt[0]
            tFmtStyle = tFmtAdditionalNameFmt[1]
        tFmtVarValue = self._formatValue(fmtVarNode[tFmtValName], skipSelf=True)
        if tFmtStyle is None:
            self._fmtVars[tFmtResValName] = tFmtVarValue
        elif tFmtStyle == "path":
            self._fmtVars[tFmtResValName] = os.path.realpath(tFmtVarValue)
        else:
            log.error("[Error] Can't find format variable style: {0}", tFmtStyle)
            raise RuntimeError("Can't construct format var")


    def _findNearsestValue(self, configKey):
        if configKey not in self._tree or not _isDictType(type(self._tree)):
            if self._parent is not None:
                return self._parent._findNearsestValue(configKey)
            else:
                log.error("[Error] Can't find '{0}' in entire config tree".format(configKey))
                raise RuntimeError("Invalid config key")
        return self._tree[configKey]

    def _formatValue(self, fmtString, skipSelf=False):
        tFmtIter = Formatter().parse(fmtString)
        tStr = ""
        for item in tFmtIter:
            tStr += item[0]
            tFmtValue = item[1]
            tFmtValueExt = item[2]
            if tFmtValue is None:
                continue
            if tFmtValue == "config":
                if tFmtValueExt not in ConfigNode._CONFIG_VARS:
                    log.error("[Error] Can't format value: {0}. Can't find config key: '{1}'"
                              .format(fmtString, tFmtValueExt))
                    raise RuntimeError("Can't format config value")
                tStr += ConfigNode._CONFIG_VARS[tFmtValueExt]
                continue
            if tFmtValue == "args":
                if tFmtValueExt not in ConfigNode._ARGS_VARS:
                    log.error("[Error] Can't format value: {0}. Can't find args key: '{1}'"
                              .format(fmtString, tFmtValueExt))
                    raise RuntimeError("Can't format config value")
                tStr += ConfigNode._ARGS_VARS[tFmtValueExt]
                continue
            elif tFmtValue == "self":
                if skipSelf:
                    tStr += "{{{0}:{1}}}".format(tFmtValue, tFmtValueExt)
                    continue
                tStr += self._findNearsestValue(tFmtValueExt)
            else:
                if tFmtValue not in self._fmtVars:
                    log.error("[Error] Can't find format value: {0} in {1}".format(tFmtValue, fmtString))
                    raise RuntimeError("Can't format config value")
                if tFmtValueExt is not None and len(tFmtValueExt) != 0:
                    log.error("[Error] Only special case can have format extension: {0}:{1} in {2}"
                              .format(tFmtValue, tFmtValueExt, fmtString))
                    raise RuntimeError("Can't format config value")
                tStr += self._fmtVars[tFmtValue]
        tFmtIter = Formatter().parse(tStr)

        hasOne = False
        for item in tFmtIter:
            if item[1] is not None:
                hasOne = True
                break
        if hasOne and fmtString == tStr:
            log.error("[Error] Can't format value: {0} (Contain unknown key)".format(tStr))
            raise RuntimeError("[Error] Can't format config value")
        elif hasOne and not skipSelf:
            return self._formatValue(tStr)
        return tStr

    def getTreeName(self):
        return "Unknown" if self._treeName is None else self._treeName

    def loadLocalConfigs(self, filename):
        if not os.path.exists(filename):
            log.error("[Error] Can't find local config file: {0}".format(filename))
            try:
                with open(filename, "w") as tFile:
                    json.dump(_LOCAL_CONFIGS_EXAMPLE, tFile, sort_keys=True, indent=4, ensure_ascii=False)
                log.info("[Info] Create local config example. Please fill it.")
            except:
                log.error("[Error] Can't create local config example")
                raise RuntimeError("Can't load local configs")
        with open(filename, "r") as tFile:
            ConfigNode._CONFIG_VARS = json.load(tFile)
        log.debug("[Info] Loaded local config file: {0}".format(filename))

    def clone(self):
        return ConfigNode(parent=self._parent, treeName="{0}-Cloned".format(self._treeName),
                            tree=copy.deepcopy(self._tree))