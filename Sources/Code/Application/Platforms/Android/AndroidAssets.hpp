#ifndef __ANDROID_ASSETS_HPP__
#define __ANDROID_ASSETS_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class AndroidAssets : public SystemLogic,
    public ETNode<ETAssets> {
public:

    AndroidAssets();
    virtual ~AndroidAssets();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssets
    Buffer ET_loadAsset(const char* assetName) override;
    JSONNode ET_loadJSONAsset(const char* assetName) override;

private:

    Buffer loadAssetImpl(const std::string& assetName);
};

#endif /* __ANDROID_ASSETS_HPP__ */