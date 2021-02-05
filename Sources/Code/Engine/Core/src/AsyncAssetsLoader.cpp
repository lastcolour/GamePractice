#include "Core/App/AsyncAssetsLoader.hpp"

AsyncAssetsLoader::AsyncAssetsLoader() {
}

AsyncAssetsLoader::~AsyncAssetsLoader() {
}

bool AsyncAssetsLoader::init() {
    ETNode<ETAsyncAssets>::connect(getEntityId());
    ETNode<ETAssetsUpdateTask>::connect(getEntityId());
    return true;
}

void AsyncAssetsLoader::deinit() {
}

void AsyncAssetsLoader::ET_updateAssets(float dt) {
    ET_PollAllEvents<ETAssets>();
}

void AsyncAssetsLoader::ET_asyncSaveLocalFile(const char* fileName, Buffer& buff) {
    ET_QueueEvent(&ETAssets::ET_saveLocalFile, fileName, buff);
}