#include "EntityTests.hpp"
#include "Entity/Entity.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/EntityLogicRegister.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"

namespace {

const char* TEST_OBJECT_NAME = "Game/Simple.json";
const char* EXTEND_OBJECT_NAME = "Game/GameBoardElem.json";

class TestLogic : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx) {
        auto classInfo = ctx.classInfo<TestLogic>("TestLogic");
    }

public:

    TestLogic() {}
    virtual ~TestLogic() {}

    bool init() override { return true; }
    void deinit() { }
};

} // namespace

TEST_F(EntityTests, CreateSimpleEntity) {
    EntityId objId = InvalidEntityId;
    ET_SendEventReturn(objId, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);
    ASSERT_NE(objId, InvalidEntityId);

    std::string name;
    ET_SendEventReturn(name, objId, &ETEntity::ET_getName);
    ASSERT_FALSE(name.empty());
}

TEST_F(EntityTests, CreateTwoSimpleEntities) {
    EntityId objId1 = InvalidEntityId;
    ET_SendEventReturn(objId1, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId objId2 = InvalidEntityId;
    ET_SendEventReturn(objId2, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    ASSERT_NE(objId1, InvalidEntityId);
    ASSERT_NE(objId2, InvalidEntityId);
    ASSERT_NE(objId1, objId2);

    std::string name1;
    ET_SendEventReturn(name1, objId1, &ETEntity::ET_getName);

    std::string name2;
    ET_SendEventReturn(name2, objId2, &ETEntity::ET_getName);

    ASSERT_EQ(name1, name1);
}

TEST_F(EntityTests, CreateInvalidEntity) {
    EntityId objId = InvalidEntityId;
    ET_SendEventReturn(objId, &ETEntityManager::ET_createEntity, "");
    ASSERT_EQ(objId, InvalidEntityId);
}

TEST_F(EntityTests, CreateParentWithChildEntity) {
    EntityId objId1 = InvalidEntityId;
    ET_SendEventReturn(objId1, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId objId2 = InvalidEntityId;
    ET_SendEventReturn(objId2, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    ET_SendEvent(objId2, &ETEntity::ET_setParent, objId1);

    EntityId parentId;
    ET_SendEventReturn(parentId, objId2, &ETEntity::ET_getParentId);

    ASSERT_EQ(parentId, objId1);

    ET_SendEvent(&ETEntityManager::ET_destroyEntity, objId1);

    ASSERT_FALSE(ET_IsExistNode<ETEntity>(objId1));
    ASSERT_FALSE(ET_IsExistNode<ETEntity>(objId2));
}

TEST_F(EntityTests, CheckChildInheritParentTransform) {
    EntityId objId1 = InvalidEntityId;
    ET_SendEventReturn(objId1, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId objId2 = InvalidEntityId;
    ET_SendEventReturn(objId2, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    ET_SendEvent(objId2, &ETEntity::ET_setParent, objId1);

    Vec3 offset = Vec3(0.5f);

    Transform childTm;
    ET_SendEventReturn(childTm, objId2, &ETEntity::ET_getTransform);
    childTm.pt += offset;
    ET_SendEvent(objId2, &ETEntity::ET_setTransform, childTm);

    Transform parentTm;
    ET_SendEventReturn(parentTm, objId1, &ETEntity::ET_getTransform);
    parentTm.pt += offset;
    ET_SendEvent(objId1, &ETEntity::ET_setTransform, parentTm);

    ET_SendEventReturn(childTm, objId2, &ETEntity::ET_getTransform);
    Vec3 diff = childTm.pt - parentTm.pt;

    ASSERT_FLOAT_EQ(diff.x, offset.x);
    ASSERT_FLOAT_EQ(diff.y, offset.x);
    ASSERT_FLOAT_EQ(diff.z, offset.x);
}

TEST_F(EntityTests, CheckChildInheritParentScale) {
    EntityId parentId;
    ET_SendEventReturn(parentId, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId childId;
    ET_SendEventReturn(childId, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    ET_SendEvent(childId, &ETEntity::ET_setParent, parentId);

    {
        Transform childTm;
        ET_SendEventReturn(childTm, childId, &ETEntity::ET_getTransform);
        childTm.scale = Vec3(0.5f);
        ET_SendEvent(childId, &ETEntity::ET_setTransform, childTm);
    }

    {
        Transform parentTm;
        ET_SendEventReturn(parentTm, parentId, &ETEntity::ET_getTransform);
        parentTm.scale = Vec3(0.5f);
        ET_SendEvent(parentId, &ETEntity::ET_setTransform, parentTm);
    }

    Transform tm;
    ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);
    EXPECT_FLOAT_EQ(tm.scale.x, 0.25f);
    EXPECT_FLOAT_EQ(tm.scale.y, 0.25f);
    EXPECT_FLOAT_EQ(tm.scale.z, 0.25f);

    {
        Transform parentTm;
        ET_SendEventReturn(parentTm, parentId, &ETEntity::ET_getTransform);
        parentTm.scale = Vec3(1.f);
        ET_SendEvent(parentId, &ETEntity::ET_setTransform, parentTm);
    }

    ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);
    EXPECT_FLOAT_EQ(tm.scale.x, 0.5f);
    EXPECT_FLOAT_EQ(tm.scale.y, 0.5f);
    EXPECT_FLOAT_EQ(tm.scale.z, 0.5f);
}

TEST_F(EntityTests, CheckChildrenDepth) {
    EntityId objId1 = InvalidEntityId;
    ET_SendEventReturn(objId1, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId objId2 = InvalidEntityId;
    ET_SendEventReturn(objId2, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId objId3 = InvalidEntityId;
    ET_SendEventReturn(objId3, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    {
        int childDepth = -1;
        ET_SendEventReturn(childDepth, objId1, &ETEntity::ET_getMaxChildrenDepth);
        EXPECT_EQ(childDepth, 0);
    }

    ET_SendEvent(objId2, &ETEntity::ET_setParent, objId1);

    {
        int childDepth = -1;
        ET_SendEventReturn(childDepth, objId1, &ETEntity::ET_getMaxChildrenDepth);
        EXPECT_EQ(childDepth, 1);
    }

    ET_SendEvent(objId3, &ETEntity::ET_setParent, objId2);

    {
        int childDepth = -1;
        ET_SendEventReturn(childDepth, objId1, &ETEntity::ET_getMaxChildrenDepth);
        EXPECT_EQ(childDepth, 2);
    }
}

TEST_F(EntityTests, CheckRegisterEntityLogics) {
    EntityLogicRegister logicRegister;
    logicRegister.registerLogic<TestLogic>();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_registerLogics, logicRegister);
    ASSERT_TRUE(res);

    JSONNode node = JSONNode::ParseString("{  \"children\" : [], \"logics\" : [ { \"type\": \"TestLogic\", \"data\" : {} } ] }");

    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntityFromJSON, node, "TestEnity");
    ASSERT_TRUE(entId.isValid());
}

TEST_F(EntityTests, CheckAddRemoveLogic) {
    EntityId objId = InvalidEntityId;
    ET_SendEventReturn(objId, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);
    ASSERT_NE(objId, InvalidEntityId);

    EntityLogicId firstLogicId = InvalidEntityLogicId;
    ET_SendEventReturn(firstLogicId, &ETEntityManager::ET_addLogicToEntity, objId, "RenderSimple");
    ASSERT_NE(firstLogicId, InvalidEntityLogicId);

    EntityLogicId secondLogicId = InvalidEntityLogicId;
    ET_SendEventReturn(secondLogicId, &ETEntityManager::ET_addLogicToEntity, objId, "RenderSimple");
    ASSERT_NE(secondLogicId, InvalidEntityLogicId);

    ASSERT_NE(firstLogicId, secondLogicId);

    ET_SendEvent(&ETEntityManager::ET_removeLogicFromEntity, objId, firstLogicId);
    ET_SendEvent(&ETEntityManager::ET_removeLogicFromEntity, objId, secondLogicId);
}

TEST_F(EntityTests, TestReflectSimpleEntity) {
    EntityId entityId;
    ET_SendEventReturn(entityId, &ETEntityManager::ET_createEntity, "Game/Simple.json");
    ASSERT_TRUE(entityId.isValid());

    EntityLogicId logicId = 0;
    MemoryStream stream;
    stream.openForWrite();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_dumpEntityLogicData, entityId, logicId, stream);
    ASSERT_TRUE(res);

    auto buffer = stream.flushToBuffer();
    stream.close();
    ASSERT_TRUE(buffer);
    stream.openForRead(buffer);

    {
        std::string geomName;
        stream.read(geomName);
        EXPECT_STREQ(geomName.c_str(), "");
    }
    {
        std::string matName;
        stream.read(matName);
        EXPECT_STREQ(matName.c_str(), "");
    }
    {
        Vec2 scale(0.f);
        stream.read(scale.x);
        stream.read(scale.y);
        EXPECT_FLOAT_EQ(scale.x, 1.f);
        EXPECT_FLOAT_EQ(scale.y, 1.f);
    }
    {
        ColorB col(0, 0, 0, 0);
        stream.read(col.r);
        stream.read(col.g);
        stream.read(col.b);
        stream.read(col.a);

        EXPECT_EQ(col.r, 255);
        EXPECT_EQ(col.g, 255);
        EXPECT_EQ(col.b, 255);
        EXPECT_EQ(col.a, 255);
    }
}