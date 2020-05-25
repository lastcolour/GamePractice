import EditorTest

from model.AssetsModel import AssetsModel

import unittest

class TestAssetsModel(unittest.TestCase):

    def testInitAssetModel(self):
        model = AssetsModel()
        self.assertTrue(model.init())
        fileTree = model.getEntitiesTree()
        self.assertIsNotNone(fileTree)

if __name__ == "__main__":
    unittest.main()