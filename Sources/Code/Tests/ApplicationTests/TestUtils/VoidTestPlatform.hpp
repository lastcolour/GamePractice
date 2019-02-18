#ifndef __VOID_TEST_PLATFORM_HPP__
#define __VOID_TEST_PLATFORM_HPP__

#include "Platform.hpp"

#include "TestUtils/VoidTestSurface.hpp"
#include "TestUtils/VoidTestLogStream.hpp"
#include "TestUtils/VoidTestAssets.hpp"

class VoidTestPlatform : public Platform {
public:

    VoidTestPlatform() = default;
    virtual ~VoidTestPlatform() = default;

    bool init() override { return retRes_init; }

    std::unique_ptr<Surface> createSurface() override { return std::move(retRes_createSurface); }
    std::unique_ptr<LogStream> createLogStream() override { return std::move(retRes_createLogStream); }
    std::unique_ptr<Assets> createAssets() override { return std::move(retRes_createAssets); }

public:

    bool retRes_init { true };
    std::unique_ptr<VoidTestSurface> retRes_createSurface { new VoidTestSurface };
    std::unique_ptr<VoidTestLogStream> retRes_createLogStream { new VoidTestLogStream };
    std::unique_ptr<VoidTestAssets> retRes_createAssets { new VoidTestAssets };
};

#endif /* __VOID_TEST_PLATFORM_HPP__ */