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
    JSONNode ET_loadJSONAsset(const char* assetName) override;
    Buffer ET_loadAsset(const char* assetName) override;
    Buffer ET_loadLocalFile(const char* assetName) override;
    JSONNode ET_loadLocalJSONFile(const char* fileName) override;
    bool ET_saveLocalFile(const char* fileName, const Buffer& buff) override;
    bool ET_removeLocalFile(const char* fileName) override;

private:

    Buffer loadFileFromDir(const std::string& dirPath, const std::string& fileName);

private:

    std::string assetRootPath;
    std::string localRootPath;
};

#endif /* __DESKTOP_ASSETS_HPP__ */