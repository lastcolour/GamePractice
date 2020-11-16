import sys
import pathlib

EDITOR_IMPORT_PATH = "{0}/App".format(pathlib.Path(__file__).parent.parent)
sys.path.append(EDITOR_IMPORT_PATH)

from model.AssetsModel import AssetsModel

from utils.AppConfig import AppConfig

from native.EditorNative import EditorNative
from native.EntityNativeLoader import EntityNativeLoader
from native.MemoryStream import MemoryStream

def converEntity(entity):
    entity.loadToNative()
    entity.unloadFromNative()

def main():
    appConfig = AppConfig()
    editorNative = EditorNative(appConfig)
    if not editorNative.init():
        print("[Error] Can't init native edtior")
        return
    entityLoader = editorNative.getEntityLoader()
    assetsModel = AssetsModel(appConfig)
    if not assetsModel.init():
        print("[Error] Can't init assets model")
        return
    fileTree = assetsModel.getEntitiesTree()
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
            entityPath = item.getRelativePath()
            print("[Info] Converting entity: '{0}'".format(entityPath))
            entity = entityLoader.loadEntity(entityPath)
            converEntity(entity)
            entity.save()

if __name__ == "__main__":
    main()