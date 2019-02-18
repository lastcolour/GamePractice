#include "GameTests.hpp"
#include "Game/Game.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "ModuleFactory.hpp"

namespace {
    const char* TEST_OBJECT_NAME = "Simple";
}

std::unique_ptr<VoidTestApplication> GameTests::app;

void GameTests::SetUp() {
}

void GameTests::TearDown() {
}

void GameTests::SetUpTestCase() {
    app.reset(new VoidTestApplication(new DesktopPlatform(0, nullptr)));
    app->retRes_createModuleFactory.reset(new ModuleFactory);
    ASSERT_TRUE(app->init());

    ASSERT_TRUE(GetEnv()->getGame());
}

void GameTests::TearDownTestCase() {
    app.reset();
}

TEST_F(GameTests, CreateSimpleGameObject) {
    auto game = GetEnv()->getGame();
    auto obj = game->createObject(TEST_OBJECT_NAME);
    ASSERT_TRUE(obj);
    ASSERT_FALSE(obj->getName().empty());
}

TEST_F(GameTests, CreateTwoSimpleObjects) {
    auto game = GetEnv()->getGame();
    auto obj1 = game->createObject(TEST_OBJECT_NAME);
    auto obj2 = game->createObject(TEST_OBJECT_NAME);

    ASSERT_TRUE(obj1);
    ASSERT_TRUE(obj2);
    ASSERT_NE(obj1->getName(), obj2->getName());
}

TEST_F(GameTests, CreateInvalidGameObject) {
    auto game = GetEnv()->getGame();
    auto obj = game->createObject("");
    ASSERT_FALSE(obj);
}