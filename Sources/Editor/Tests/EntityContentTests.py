import EditorTest

import unittest

from model.AssetsModel import AssetsModel

from utils.AppConfig import AppConfig

from native.EditorNative import EditorNative
from native.EntityNativeLoader import EntityNativeLoader
from native.MemoryStream import MemoryStream

def _dumpEntityToMemoryStream(entity, stream):
    stream.writeString(entity._name)
    stream.writeBool(entity._isInternal)
    if entity._childId is not None:
        stream.writeInt(entity._childId)
    else:
        stream.writeInt(-1)
    stream.writeInt(len(entity._logics) + 1)
    entity._tmLogic.writeToStream(stream)
    for logic in entity._logics:
        logic.writeToStream(stream)
    stream.writeInt(len(entity._children))
    for child in entity._children:
        _dumpEntityToMemoryStream(child, stream)

class EntityContentTests(unittest.TestCase):

    ASSETS = None
    EDITOR = None

    @classmethod
    def setUpClass(cls):
        cls.ASSETS = AssetsModel(AppConfig())
        if not cls.ASSETS.init():
            cls.ASSETS = None
            raise RuntimeError("Can't init Assets model")
        cls.EDITOR = EditorNative(AppConfig())
        if not cls.EDITOR.init():
            cls.EDITOR = None
            raise RuntimeError("Can't init editor native")

    @classmethod
    def tearDownClass(cls):
        if cls.EDITOR is not None:
            cls.EDITOR.deinit()
            cls.EDITOR = None

    def _getAssets(self):
        return EntityContentTests.ASSETS

    def _getEntityLoader(self):
        return EntityContentTests.EDITOR.getEntityLoader()

    def testLoadOfAllEntities(self):
        fileTree = self._getAssets().getEntitiesTree()
        loadList = []
        for item in fileTree.getChildren():
            loadList.append(item)
        while len(loadList) > 0:
            item = loadList[-1]
            loadList.pop()
            if item.isDir():
                for childItem in item.getChildren():
                    loadList.append(childItem)
            else:
                entity = self._getEntityLoader().loadEntity(item.getRelativePath())
                self.assertIsNotNone(entity)
                self.assertTrue(entity.loadToNative())
                entity.unloadFromNative()

if __name__ == "__main__":
    unittest.main()