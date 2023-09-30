#ifndef __ENGINE_TESTS_HPP__
#define __ENGINE_TESTS_HPP__

#include "Entity.hpp"

#include <gtest/gtest.h>

void HACK_ASSERT_TRUE(bool value);

enum class ETestAppModules {
    Void = 0,
    CheckAssets,
    CheckRender,
    CheckEntities,
    CheckSound,
    CheckGame,
    CheckUI
};


class EngineTests : public ::testing::Test {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::Void);
    }

    static void TearDownTestCase();

    void TearDown() override;

protected:

    static void CreateTestApp(ETestAppModules appModules);

protected:

    Entity* createVoidObject();

private:

    std::vector<EntityId> tempObject;
};

#endif /* __ENGINE_TESTS_HPP__ */