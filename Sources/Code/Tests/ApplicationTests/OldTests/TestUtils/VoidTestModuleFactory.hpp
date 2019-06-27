#ifndef __VOID_TEST_MODULE_FACTORY_HPP__
#define __VOID_TEST_MODULE_FACTORY_HPP__

#include "ModuleFactory.hpp"

#include "Platform.hpp"

#include "TestUtils/VoidTestSurface.hpp"
#include "TestUtils/VoidTestGame.hpp"
#include "TestUtils/VoidTestRender.hpp"
#include "TestUtils/VoidTestLogger.hpp"
#include "TestUtils/VoidTestAssets.hpp"

class OnlyVoidGameModuleFactory : public ModuleFactory {
public:

    OnlyVoidGameModuleFactory() = default;
    virtual ~OnlyVoidGameModuleFactory() = default;

    std::unique_ptr<Game> createGame() { return std::move(retRes_createGame); }

public:

    std::unique_ptr<VoidTestGame> retRes_createGame { new VoidTestGame };
};

class VoidTestModuleFactory : public ModuleFactory {
public:

    VoidTestModuleFactory() = default;
    virtual ~VoidTestModuleFactory() = default;

public:

    std::unique_ptr<VoidTestLogger> retRes_createLogger { new VoidTestLogger };
    std::unique_ptr<VoidTestAssets> retRes_createAssets { new VoidTestAssets };
    std::unique_ptr<VoidTestSurface> retRes_createSurface { new VoidTestSurface };
    std::unique_ptr<VoidTestRender> retRes_createRender { new VoidTestRender };
    std::unique_ptr<VoidTestGame> retRes_createGame { new VoidTestGame };

protected:

    std::unique_ptr<Logger> createLogger() override { return std::move(retRes_createLogger); }
    std::unique_ptr<Assets> createAssets() override { return std::move(retRes_createAssets); }
    std::unique_ptr<Surface> createSurface() override { return std::move(retRes_createSurface); }
    std::unique_ptr<Render> createRender() override { return std::move(retRes_createRender); }
    std::unique_ptr<Game> createGame() override { return std::move(retRes_createGame); }
};

#endif /* __VOID_TEST_MODULE_FACTORY_HPP__ */