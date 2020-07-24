#include "AsyncAssetsLoader.hpp"

AsyncAssetsLoader::AsyncAssetsLoader() {
}

AsyncAssetsLoader::~AsyncAssetsLoader() {
}

bool AsyncAssetsLoader::init() {
    ETNode<ETAssetsUpdateTask>::connect(getEntityId());
    return true;
}

void AsyncAssetsLoader::deinit() {
}

void AsyncAssetsLoader::ET_updateAssets() {
}