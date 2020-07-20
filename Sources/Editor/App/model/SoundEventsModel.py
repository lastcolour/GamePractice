import json
import os

from utils.Log import Log

class RootNode:
    def __init__(self):
        self._children = []

    def isDir(self):
        return True

    def getChildren(self):
        return self._children

    def getRelativePath(self):
        return ""

    def __iter__(self):
        for child in self._children:
            yield child

class SoundEvent:
    def __init__(self):
        self._name = None

    def isDir(self):
        return False

    def getBaseName(self):
        return self._name

    def getRelativePath(self):
        return self._name

class SoundEventsModel:
    def __init__(self, appConfig):
        self._appConfig = appConfig
        self._root = RootNode()

    def init(self):
        eventsFile = "{0}/Sounds/Events.json".format(self._appConfig.getAssetsRootPath())
        if not os.path.exists(eventsFile):
            Log.error("[SoundEventsModel:init] Can't find events config: '{0}'".format(eventsFile))
            return False
        with open(eventsFile, 'r') as tFile:
            data = json.load(tFile)
        for item in data["Events"]:
            event = SoundEvent()
            event._name = item["name"]
            if len(event._name) == 0:
                continue
            self._root._children.append(event)
        return True

    def getResourceTree(self):
        return self._root