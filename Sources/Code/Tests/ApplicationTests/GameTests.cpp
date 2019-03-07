#include "GameTests.hpp"
#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Environment.hpp"

namespace {
    const char* TEST_OBJECT_NAME = "Simple";
}

TEST_F(GameTests, CreateSimpleGameObject) {
    EntityId objId = InvalidEntityId;
    ET_SendEventReturn(objId, &ETGame::ET_createGameObject, TEST_OBJECT_NAME);
    ASSERT_NE(objId, InvalidEntityId);

    std::string name;
    ET_SendEventReturn(name, objId, &ETGameObject::ET_getName);
    ASSERT_FALSE(name.empty());
}

TEST_F(GameTests, CreateTwoSimpleObjects) {
    EntityId objId1 = InvalidEntityId;
    ET_SendEventReturn(objId1, &ETGame::ET_createGameObject, TEST_OBJECT_NAME);

    EntityId objId2 = InvalidEntityId;
    ET_SendEventReturn(objId2, &ETGame::ET_createGameObject, TEST_OBJECT_NAME);

    ASSERT_NE(objId1, InvalidEntityId);
    ASSERT_NE(objId2, InvalidEntityId);
    ASSERT_NE(objId1, objId2);

    std::string name1;
    ET_SendEventReturn(name1, objId1, &ETGameObject::ET_getName);

    std::string name2;
    ET_SendEventReturn(name2, objId2, &ETGameObject::ET_getName);

    ASSERT_EQ(name1, name1);
}

TEST_F(GameTests, CreateInvalidGameObject) {
    EntityId objId = InvalidEntityId;
    ET_SendEventReturn(objId, &ETGame::ET_createGameObject, "");
    ASSERT_EQ(objId, InvalidEntityId);
}