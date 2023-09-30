#ifndef __ANDROID_ASSETS_HPP__
#define __ANDROID_ASSETS_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETAssets.hpp"

class AndroidAssets : public SystemLogic,
    public ETNode<ETAssets> {
public:

    AndroidAssets();
    virtual ~AndroidAssets();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssets
    Memory::Buffer ET_loadAsset(const char* assetName) override;
    JSONNode ET_loadJSONAsset(const char* assetName) override;
    Memory::Buffer ET_loadLocalFile(const char* fileName) override;
    JSONNode ET_loadLocalJSONFile(const char* fileName) override;
    bool ET_saveLocalFile(const char* fileName, const Memory::Buffer& buff) override;
    bool ET_removeLocalFile(const char* fileName) override;
    bool ET_isLocalFileExists(const char* fileName) const override;

private:

    Memory::Buffer loadAssetImpl(const std::string& assetName);
};

#endif /* __ANDROID_ASSETS_HPP__ */