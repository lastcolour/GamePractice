#ifndef __ET_ASSETS_HPP__
#define __ET_ASSETS_HPP__

#include "Core/Buffer.hpp"
#include "Core/JSONNode.hpp"

struct ETAssets {
    virtual ~ETAssets() = default;
    virtual Buffer ET_loadAsset(const char* assetName) = 0;
    virtual JSONNode ET_loadJSONAsset(const char* assetName) = 0;
    virtual Buffer ET_loadLocalFile(const char* fileName) = 0;
    virtual JSONNode ET_loadLocalJSONFile(const char* fileName) = 0;
    virtual bool ET_saveLocalFile(const char* fileName, const Buffer& buff) = 0;
    virtual bool ET_removeLocalFile(const char* fileName) = 0;
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
};

#endif /* __ET_ASSETS_HPP__ */