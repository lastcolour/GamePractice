#ifndef __VOID_TEST_ASSETS_HPP__
#define __VOID_TEST_ASSETS_HPP__

#include "Assets.hpp"

class VoidTestAssets : public Assets {
public:

    VoidTestAssets() = default;
    virtual ~VoidTestAssets() = default;
    bool init() override { return retRes_init; }
    Buffer loadAsset(const std::string& assetName) override { return Buffer(); }

public:

    bool retRes_init { true };
};

#endif /* __VOID_TEST_ASSETS_HPP__ */