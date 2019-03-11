#ifndef __DESKTOP_ASSETS_HPP__
#define __DESKTOP_ASSETS_HPP__

#include "Platform.hpp"

class DesktopAssets : public Assets {
public:

    DesktopAssets() = default;
    virtual ~DesktopAssets() = default;

    // ETAssets
    JSONNode ET_loadJSONAsset(const std::string& assetName) override;
    Buffer ET_loadAsset(const std::string& assetName) override;

protected:

    // SystemLogic
    bool onInit() override;
    bool onShouldRun() override { return true; }
    void onUpdate(float dt) override { (void)dt; }

private:

     Buffer loadAssetImpl(const std::string& assetName);

private:

    std::string assetRootPath;
};

#endif /* __DESKTOP_ASSETS_HPP__ */