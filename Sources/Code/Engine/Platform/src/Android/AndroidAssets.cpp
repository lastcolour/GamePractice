#include "Android/AndroidAssets.hpp"
#include "Android/AndroidPlatformHandler.hpp"
#include "Core/JSONNode.hpp"
#include "Core/ETLogger.hpp"
#include "Core/TimePoint.hpp"
#include "FileUtils.hpp"

#include <android/asset_manager.h>
#include <sys/stat.h>

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
    auto normalAssetName = FileUtils::NormilizeAssetName(assetName);
    Buffer buff;
    if(normalAssetName.empty()) {
        return buff;
    }
    ET_SendEventReturn(buff, &ETAssetsCacheManager::ET_getAssetFromCache, normalAssetName.c_str());
    if(buff) {
        return buff;
    }

    auto loadStartT = TimePoint::GetNowTime();

    buff = loadAssetImpl(normalAssetName);

    if(buff) {
        ET_SendEvent(&ETAssetsCacheManager::ET_putAssetToCache, normalAssetName.c_str(), buff);
    }

    if(buff) {
        float msValue = -static_cast<int>(loadStartT.getMiliSecElapsedFrom(TimePoint::GetNowTime()));
        LogDebug("[AndroidAssets::ET_loadAsset] Loaded file '%s' in %.1f ms", assetName, msValue);
    }

    return buff;
}

Buffer AndroidAssets::loadAssetImpl(const std::string& assetName) {
    {
        // Load from local file
        std::filesystem::path assetPath = assetName;
        if(FileUtils::IsFileExists(assetPath)) {
            return FileUtils::LoadFile(assetPath);
        }
    }

    // Load from apk
    AAssetManager* assetManager = GetAndroindPlatformHandler()->getAssetsManager();
    AAsset* androidAsset = AAssetManager_open(assetManager, assetName.c_str(), AASSET_MODE_BUFFER);
    if(!androidAsset) {
        LogError("[AndroidAssets::loadAssetImpl] Can't open asset: '%s'", assetName);
        return Buffer();
    }
    off_t assetSize = AAsset_getLength(androidAsset);
    Buffer buff(assetSize);
    if(!buff) {
        LogError("[AndroidAssets::loadAssetImpl] Can't allocate buffer of size %d bytes to load asset '%s'",
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
        LogError("[AndroidAssets::loadAssetImpl] Unknow platform error when load asset '%s'", assetName);
        AAsset_close(androidAsset);
        return Buffer();
    }
    AAsset_close(androidAsset);
    return buff;
}

Buffer AndroidAssets::ET_loadLocalFile(const char* fileName) {
    return FileUtils::LoadFileFromDir(GetAndroindPlatformHandler()->getInternalPath(), fileName);
}

JSONNode AndroidAssets::ET_loadLocalJSONFile(const char* fileName) {
    auto buffer = ET_loadLocalFile(fileName);
    if(!buffer) {
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[AndroidAssets::ET_loadLocalJSONFile] Can't parse file: '%s'", fileName);
        return JSONNode();
    }
    return rootNode;
}

bool AndroidAssets::ET_saveLocalFile(const char* fileName, const Buffer& buff) {
    return FileUtils::SaveFileToDir(GetAndroindPlatformHandler()->getInternalPath(), fileName, buff);
}

bool AndroidAssets::ET_removeLocalFile(const char* fileName) {
    return FileUtils::RemoveFileFromDir(GetAndroindPlatformHandler()->getInternalPath(), fileName);
}

bool AndroidAssets::ET_isLocalFileExists(const char* fileName) const {
    auto path = FileUtils::CombinePath(GetAndroindPlatformHandler()->getInternalPath(), fileName);
    return FileUtils::IsFileExists(path);
}