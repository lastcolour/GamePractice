#ifndef __DESKTOP_ASSETS_HPP__
#define __DESKTOP_ASSETS_HPP__

#include "Core/ETAssets.hpp"
#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"

#include <filesystem>

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
    Memory::Buffer ET_loadAsset(const char* assetName) override;
    Memory::Buffer ET_loadLocalFile(const char* assetName) override;
    JSONNode ET_loadLocalJSONFile(const char* fileName) override;
    bool ET_saveLocalFile(const char* fileName, const Memory::Buffer& buff) override;
    bool ET_removeLocalFile(const char* fileName) override;
    bool ET_isLocalFileExists(const char* fileName) const override;

private:

    Memory::Buffer loadFileFromDir(const std::filesystem::path& dirPath, const std::string& fileName);

private:

    std::filesystem::path assetRootPath;
    std::filesystem::path localRootPath;
};

#endif /* __DESKTOP_ASSETS_HPP__ */