#ifndef __DESKTOP_ASSETS_HPP__
#define __DESKTOP_ASSETS_HPP__

#include "Assets.hpp"

class DesktopAssets : public Assets {
public:

    DesktopAssets() = default;
    virtual ~DesktopAssets() = default;

    bool init() override;
    Buffer loadAsset(const std::string& assetName) override;

private:

     Buffer loadAssetImpl(const std::string& assetName);

private:

    std::string assetRootPath;
};

#endif /* __DESKTOP_ASSETS_HPP__ */