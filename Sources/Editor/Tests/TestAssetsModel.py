import EditorTest

from model.AssetsModel import AssetsModel

from utils.AppConfig import AppConfig

from native.EditorNative import EditorNative
from native.EntityNativeLoader import EntityNativeLoader

import unittest
import shutil
import os

class TestAssetsModel(unittest.TestCase):

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
        return TestAssetsModel.ASSETS

    def setUp(self):
        self._toRemove = []

    def tearDown(self):
        for item in self._toRemove:
            if not os.path.exists(item):
                continue
            if os.path.isdir(item):
                shutil.rmtree(item)
            else:
                os.remove(item)

    def testInitAssetModel(self):
        fileTree = self._getAssets().getEntitiesTree()
        self.assertIsNotNone(fileTree)
        self.assertTrue(fileTree.isDir())
        self.assertTrue(len(fileTree.getChildren()) > 0)
        item = fileTree.getChildren()[0]
        self.assertFalse(item.isRoot())
        self.assertGreater(len(item.getFullPath()), len(item.getRelativePath()))
        self.assertGreaterEqual(len(item.getRelativePath()), len(item.getBaseName()))

    def testCreateNewDir(self):
        fileTree = self._getAssets().getEntitiesTree()
        rootItem = fileTree.getChildren()[0]
        newItem = self._getAssets().createNewDir(rootItem, "TestDir")
        if newItem != None:
            self._toRemove.append(newItem.getFullPath())
        self.assertIsNotNone(newItem)
        itemPath = newItem.getFullPath()
        self.assertTrue(os.path.exists(itemPath))
        self.assertTrue(os.path.isdir(itemPath))
        self.assertTrue(self._getAssets().removeNode(newItem))
        self.assertFalse(os.path.exists(itemPath))

    def testCreateNewEntity(self):
        fileTree = self._getAssets().getEntitiesTree()
        rootItem = fileTree.getChildren()[0]
        newItem = self._getAssets().createNewEntity(rootItem, "TestEntity.json")
        if newItem != None:
            self._toRemove.append(newItem.getFullPath())
        self.assertIsNotNone(newItem)
        itemPath = newItem.getFullPath()
        self.assertTrue(os.path.exists(itemPath))
        self.assertFalse(os.path.isdir(itemPath))
        self._toRemove.append(itemPath)

        loader = EntityNativeLoader()
        self.assertTrue(loader.init())

        entity = loader.loadEntity(newItem.getRelativePath())
        self.assertIsNotNone(entity)

        self.assertTrue(self._getAssets().removeNode(newItem))
        self.assertFalse(os.path.exists(itemPath))

    def testRenameEntity(self):
        fileTree = self._getAssets().getEntitiesTree()
        rootItem = fileTree.getChildren()[0]
        newItem = self._getAssets().createNewEntity(rootItem, "TestEntity.json")
        self.assertIsNotNone(newItem)
        itemPath = newItem.getFullPath()
        self.assertTrue(os.path.exists(itemPath))
        self._toRemove.append(itemPath)

        self.assertTrue(self._getAssets().renameNode(newItem, "TestEntityFile.json"))
        newItemPath = newItem.getFullPath()
        self._toRemove.append(newItemPath)
        self.assertNotEqual(itemPath, newItemPath)
        self.assertTrue(os.path.exists(newItemPath))
        self.assertFalse(os.path.exists(itemPath))

        loader = EntityNativeLoader()
        self.assertTrue(loader.init())

        entity = loader.loadEntity(newItem.getRelativePath())
        self.assertIsNotNone(entity)

        self.assertTrue(self._getAssets().removeNode(newItem))
        self.assertFalse(os.path.exists(itemPath))

if __name__ == "__main__":
    unittest.main()