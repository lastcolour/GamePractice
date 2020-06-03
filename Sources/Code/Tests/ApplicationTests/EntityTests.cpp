#include "EntityTests.hpp"
#include "Entity/Entity.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/EntityLogicRegister.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"
#include "Core/StringFormat.hpp"

#include <set>

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

    const char* transformDataStr = "{ \"pos\": {\"x\": 0.1, \"y\": 0.2, \"z\": 0.3}, \"scale\": {\"x\": 0.4, \"y\" :0.5, \"z\": 0.6}, \"rot\": {\"x\": 0, \"y\": 0, \"z\": 0, \"w\": 1} }";
    const char* childDataStr = "";
    const char* logicsDataStr = "{ \"type\": \"TestLogic\", \"data\" : {} }";
    std::string resStr = StringFormat("{ \"transform\": %s,  \"children\" : [ %s ], \"logics\" : [ %s ] }", transformDataStr, childDataStr, logicsDataStr);
    JSONNode node = JSONNode::ParseString(resStr.c_str());

    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntityFromJSON, node, "TestEnity");
    ASSERT_TRUE(entId.isValid());

    Transform tm;
    ET_SendEventReturn(tm, entId, &ETEntity::ET_getTransform);

    ASSERT_FLOAT_EQ(tm.pt.x, 0.1f);
    ASSERT_FLOAT_EQ(tm.pt.y, 0.2f);
    ASSERT_FLOAT_EQ(tm.pt.z, 0.3f);

    ASSERT_FLOAT_EQ(tm.scale.x, 0.4f);
    ASSERT_FLOAT_EQ(tm.scale.y, 0.5f);
    ASSERT_FLOAT_EQ(tm.scale.z, 0.6f);

    ASSERT_FLOAT_EQ(tm.quat.x, 0.f);
    ASSERT_FLOAT_EQ(tm.quat.y, 0.f);
    ASSERT_FLOAT_EQ(tm.quat.z, 0.f);
    ASSERT_FLOAT_EQ(tm.quat.w, 1.f);

    MemoryStream stream;
    stream.openForWrite();

    res = false;
    EntityLogicId logicId = 0;
    EntityLogicValueId valueId = 1;
    ET_SendEventReturn(res, &ETEntityManager::ET_readEntityLogicData, entId, logicId, valueId, stream);

    ASSERT_TRUE(res);

    stream.reopenForRead();

    {
        float xVal = 0.f;
        stream.read(xVal);
        ASSERT_FLOAT_EQ(xVal, 0.1f);
    }
    {
        float yVal = 0.f;
        stream.read(yVal);
        ASSERT_FLOAT_EQ(yVal, 0.2f);
    }
    {
        float zVal = 0.f;
        stream.read(zVal);
        ASSERT_FLOAT_EQ(zVal, 0.3f);
    }

    stream.reopenForWrite();
    stream.write(0.3f);
    stream.write(0.2f);
    stream.write(0.1f);

    stream.reopenForRead();

    res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_writeEntityLogicData, entId, logicId, valueId, stream);

    ASSERT_TRUE(res);

    ET_SendEventReturn(tm, entId, &ETEntity::ET_getTransform);

    ASSERT_FLOAT_EQ(tm.pt.x, 0.3f);
    ASSERT_FLOAT_EQ(tm.pt.y, 0.2f);
    ASSERT_FLOAT_EQ(tm.pt.z, 0.1f);
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

TEST_F(EntityTests, CheckRegisteredLogics) {
    std::vector<const char*> registeredLogics;
    ET_SendEvent(&ETEntityManager::ET_getRegisteredLogics, registeredLogics);
    ASSERT_TRUE(registeredLogics.size() > 0);

    std::set<const char*> logics;
    for(auto name : registeredLogics) {
        ASSERT_TRUE(name && name[0]);
        logics.insert(name);
    }

    ASSERT_EQ(logics.size(), registeredLogics.size());
}

TEST_F(EntityTests, TestReflectSimpleEntity) {
    EntityId entityId;
    ET_SendEventReturn(entityId, &ETEntityManager::ET_createEntity, "Game/Simple.json");
    ASSERT_TRUE(entityId.isValid());

    EntityLogicId logicId = 1;
    MemoryStream stream;
    stream.openForWrite();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_readEntityLogicData, entityId, logicId, AllEntityLogicValueId, stream);
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

TEST_F(EntityTests, TestAddElementToUILayout) {
    EntityId entityId;
    ET_SendEventReturn(entityId, &ETEntityManager::ET_createEntity, "Game/Simple.json");
    ASSERT_TRUE(entityId.isValid());

    EntityLogicId logicId = InvalidEntityLogicId;
    ET_SendEventReturn(logicId, &ETEntityManager::ET_addLogicToEntity, entityId, "UILayout");
    ASSERT_NE(logicId, InvalidEntityLogicId);

    EntityLogicValueId valueId = 4;
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_addEntityLogicArrayElement, entityId, logicId, valueId);
    ASSERT_TRUE(res);

    MemoryStream stream;
    stream.openForWrite();

    ET_SendEventReturn(res, &ETEntityManager::ET_readEntityLogicData, entityId, logicId, valueId, stream);
    ASSERT_TRUE(res);

    stream.reopenForRead();
    {
        int size = 0;
        stream.read(size);
        ASSERT_EQ(size, 1);
    }
    {
        std::string val;
        stream.read(val);
        ASSERT_STREQ(val.c_str(), "");
    }
}
