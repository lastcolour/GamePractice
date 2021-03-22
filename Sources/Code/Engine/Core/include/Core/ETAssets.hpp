#ifndef __ET_ASSETS_HPP__
#define __ET_ASSETS_HPP__

#include "Core/JSONNode.hpp"

struct AsyncLoadRequest;

struct ETAssets {
    virtual ~ETAssets() = default;
    virtual Buffer ET_loadAsset(const char* assetName) = 0;
    virtual JSONNode ET_loadJSONAsset(const char* assetName) = 0;
    virtual Buffer ET_loadLocalFile(const char* fileName) = 0;
    virtual JSONNode ET_loadLocalJSONFile(const char* fileName) = 0;
    virtual bool ET_saveLocalFile(const char* fileName, const Buffer& buff) = 0;
    virtual bool ET_removeLocalFile(const char* fileName) = 0;
    virtual bool ET_isLocalFileExists(const char* fileName) const = 0;
};

struct ETAssetsCacheManager {
    virtual ~ETAssetsCacheManager() = default;
    virtual Buffer ET_getAssetFromCache(const char* assetName) = 0;
    virtual void ET_putAssetToCache(const char* assetName, const Buffer& buff) = 0;
    virtual void ET_setCacheLifetime(float seconds) = 0;
    virtual float ET_getCacheLifetime() const = 0;
    virtual void ET_clear() = 0;
};

struct ETAssetsSetup {
    virtual ~ETAssetsSetup() = default;
    virtual void ET_getAssetsSearchPath(std::string& searchPath) = 0;
    virtual void ET_getLocalDataRootPath(std::string& localDataRooPath) = 0;
};

struct ETAsyncAssets {
    virtual ~ETAsyncAssets() = default;
    virtual void ET_asyncSaveLocalFile(const char* fileName, Buffer& buff) = 0;
    virtual void ET_asyncLoadLocalFile(const char* fileName, std::function<void(Buffer&)> callback) = 0;
    virtual void ET_asyncLoadAsset(const char* fileName, std::function<void(Buffer&)> callback) = 0;
    virtual void ET_processAsyncLoadRequest(const AsyncLoadRequest& req) = 0;
};

#endif /* __ET_ASSETS_HPP__ */