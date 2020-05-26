import EditorTest

from model.AssetsModel import AssetsModel
from utils.AppConfig import AppConfig

import unittest

class TestAssetsModel(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ASSETS = AssetsModel(AppConfig())
        if not cls.ASSETS.init():
            raise RuntimeError("Can't init Assets model")

    def _getAssets(self):
        return TestAssetsModel.ASSETS

    def testInitAssetModel(self):
        fileTree = self._getAssets().getEntitiesTree()
        self.assertIsNotNone(fileTree)
        self.assertTrue(fileTree.isDir())
        self.assertTrue(len(fileTree.getChildren()) > 0)
        for item in fileTree:
            print(item.getFullPath())

if __name__ == "__main__":
    unittest.main()