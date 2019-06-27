#ifndef __DESKTOP_ASSETS_HPP__
#define __DESKTOP_ASSETS_HPP__

#include "ETApplicationInterfaces.hpp"
#include "Core/SystemLogic.hpp"

class DesktopAssets : public SystemLogic,
    public ETNode<ETAssets> {
public:

    DesktopAssets() = default;
    virtual ~DesktopAssets() = default;

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssets
    JSONNode ET_loadJSONAsset(const std::string& assetName) override;
    Buffer ET_loadAsset(const std::string& assetName) override;

private:

     Buffer loadAssetImpl(const std::string& assetName);

private:

    std::string assetRootPath;
};

#endif /* __DESKTOP_ASSETS_HPP__ */