#include "GameTests.hpp"
#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Environment.hpp"

namespace {
    const char* TEST_OBJECT_NAME = "Simple";
}

TEST_F(GameTests, CreateSimpleGameObject) {
    auto objId = ET_SendEventReturn(&ETGame::ET_createGameObject, TEST_OBJECT_NAME);
    ASSERT_NE(objId, InvalidEntityId);
    auto& name = ET_SendEventReturn(objId, &ETGameObject::ET_getName);
    ASSERT_FALSE(name.empty());
}

TEST_F(GameTests, CreateTwoSimpleObjects) {
    auto objId1 = ET_SendEventReturn(&ETGame::ET_createGameObject, TEST_OBJECT_NAME);
    auto objId2 = ET_SendEventReturn(&ETGame::ET_createGameObject, TEST_OBJECT_NAME);

    ASSERT_NE(objId1, InvalidEntityId);
    ASSERT_NE(objId2, InvalidEntityId);
    ASSERT_NE(objId1, objId2);

    auto& name1 = ET_SendEventReturn(objId1, &ETGameObject::ET_getName);
    auto& name2 = ET_SendEventReturn(objId2, &ETGameObject::ET_getName);

    ASSERT_NE(name1, name1);
}

TEST_F(GameTests, CreateInvalidGameObject) {
    auto objId = ET_SendEventReturn(&ETGame::ET_createGameObject, "");
    ASSERT_EQ(objId, InvalidEntityId);
}