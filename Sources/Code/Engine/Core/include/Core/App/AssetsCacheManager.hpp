#ifndef __APP_CACHE_MANAGER_HPP__
#define __APP_CACHE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETTasks.hpp"
#include "Core/TimePoint.hpp"

#include <mutex>

class AssetsCacheManager : SystemLogic,
    public ETNode<ETAssetsUpdateTask>,
    public ETNode<ETAssetsCacheManager> {
private:

    struct AssetCacheNode {
        Buffer buff;
        float lifetime;
    };

private:

    using AssetsCacheMapT = std::unordered_map<std::string, AssetCacheNode>;

public:

    AssetsCacheManager();
    virtual ~AssetsCacheManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssetsCacheManager
    Buffer ET_getAssetFromCache(const char* assetName) override;
    void ET_putAssetToCache(const char* assetName, const Buffer& buff) override;
    void ET_setCacheLifetime(float seconds) override;
    float ET_getCacheLifetime() const override;
    void ET_clear() override;

    // ETAssetsUpdateTask
    void ET_updateAssets(float dt) override;

private:

    mutable std::mutex mutex;
    AssetsCacheMapT assetsCacheMap;
    float assetsLifetime;
};

#endif /* __APP_CACHE_MANAGER_HPP__ */