#include "Platforms/Android/AndroidAssets.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"
#include "Core/JSONNode.hpp"

#include <fstream>

#include <android/asset_manager.h>

namespace {

const off_t READ_CHUNK_SIZE = 1024 * 1024;

} // namespace

AndroidAssets::AndroidAssets() {
}

AndroidAssets::~AndroidAssets() {
}

bool AndroidAssets::init() {
    ETNode<ETAssets>::connect(getEntityId());
    return true;
}

void AndroidAssets::deinit() {
    ETNode<ETAssets>::disconnect();
}

JSONNode AndroidAssets::ET_loadJSONAsset(const char* assetName) {
    auto buffer = ET_loadAsset(assetName);
    if(!buffer) {
        LogError("[AndroidAssets::loadJSONAsset] Can't load asset from: %s", assetName);
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[AndroidAssets::loadJSONAsset] Can't parse asset: %s", assetName);
        return JSONNode();
    }
    return rootNode;
}

Buffer AndroidAssets::ET_loadAsset(const char* assetName) {
    Buffer buff;
    if(!assetName || !assetName[0]) {
        return buff;
    }
    ET_SendEventReturn(buff, &ETAssetsCacheManager::ET_getAssetFromCache, assetName);
    if (buff) {
        return buff;
    }

    auto loadStartT = std::chrono::high_resolution_clock::now();

    buff = loadAssetImpl(assetName);

    if(buff) {
        ET_SendEvent(&ETAssetsCacheManager::ET_putAssetToCache, assetName, buff);
    }

    if(buff) {
        LogDebug("[AndroidAssets::ET_loadAsset] Loaded file '%s' in %d ms", assetName,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - loadStartT).count());
    }

    return buff;
}

Buffer AndroidAssets::loadAssetImpl(const std::string& assetName) {
    // Load from local file
    std::ifstream fin(assetName, std::ios::binary | std::ios::ate);
    if(fin.good() && fin.is_open()) {
        long int fileSize = fin.tellg();
        if(fileSize != -1) {
            fin.seekg(0u, std::ios::beg);
            Buffer buff(fileSize);
            if (buff && fin.read(static_cast<char*>(buff.getWriteData()), fileSize)) {
                LogDebug("[AndroidAssets::loadAssetImpl] Load asset from local file: %s", assetName);
                return buff;
            }
        }
    }

    // Load from apk
    AAssetManager* assetManager = GetAndroindPlatformHandler()->getAssetsManager();
    AAsset* androidAsset = AAssetManager_open(assetManager, assetName.c_str(), AASSET_MODE_BUFFER);
    if(!androidAsset) {
        LogError("[AndroidAssets::loadAssetImpl] Can't open asset: %s", assetName);
        return Buffer();
    }
    off_t assetSize = AAsset_getLength(androidAsset);
    Buffer buff(assetSize);
    if(!buff) {
        LogError("[AndroidAssets::loadAssetImpl] Can't allocate buffer of size %d to load asset %s",
            assetSize, assetName);
        AAsset_close(androidAsset);
        return Buffer();
    }
    off_t currOffset = 0;
    off_t readSize = 0;
    while((readSize = AAsset_read(androidAsset, static_cast<char*>(buff.getWriteData()) + currOffset, ::READ_CHUNK_SIZE)) > 0) {
        currOffset += readSize;
    }
    if(readSize < 0) {
        LogError("[AndroidAssets::loadAssetImpl] Unknow platform error when load asset %s", assetName);
        AAsset_close(androidAsset);
        return Buffer();
    }
    AAsset_close(androidAsset);
    return buff;
}