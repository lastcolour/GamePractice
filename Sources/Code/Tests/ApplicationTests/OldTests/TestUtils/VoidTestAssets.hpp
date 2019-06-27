#ifndef __VOID_TEST_ASSETS_HPP__
#define __VOID_TEST_ASSETS_HPP__

#include "Platform.hpp"
#include "Core/JSONNode.hpp"

class VoidTestAssets : public Assets {
public:

    VoidTestAssets() = default;
    virtual ~VoidTestAssets() = default;

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssets
    Buffer ET_loadAsset(const std::string& assetName) override { return Buffer(); }
    JSONNode ET_loadJSONAsset(const std::string& assetName) override { return JSONNode(); }

protected:

    // SystemLogic
    bool onInit() override { return retRes_init; }
    bool onShouldRun() override { return true; }
    void onUpdate(float dt) override {}

public:

    bool retRes_init { true };
};

#endif /* __VOID_TEST_ASSETS_HPP__ */