#ifndef __APP_CACHE_MANAGER_HPP__
#define __APP_CACHE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class AssetsCacheManager : SystemLogic,
    public ETNode<ETAssetsCacheManager>,
    public ETNode<ETTimerEvents>
{
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

    // ETTimerEvent
    void ET_onTick(float dt) override;

private:
    
    AssetsCacheMapT assetsCacheMap;
    float assetsLifetime;
};

#endif /* __APP_CACHE_MANAGER_HPP__ */