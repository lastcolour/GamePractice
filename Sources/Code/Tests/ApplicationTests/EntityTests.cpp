#include "EntityTests.hpp"
#include "Entity.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"
#include "Logics/UIAnimationSequence.hpp"

#include <set>

namespace {

const char* TEST_OBJECT_NAME = "Entities/Game/Simple.json";

class TestLogic : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx) {
        ctx.classInfo<TestLogic>("TestLogic");
    }

public:

    TestLogic() {}
    virtual ~TestLogic() {}

    void init() override {}
    void deinit() override { }
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

TEST_F(EntityTests, CheckChildInheritParentRotation) {
    EntityId parentId;
    ET_SendEventReturn(parentId, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    EntityId childId;
    ET_SendEventReturn(childId, &ETEntityManager::ET_createEntity, TEST_OBJECT_NAME);

    ET_SendEvent(childId, &ETEntity::ET_setParent, parentId);

    {
        Transform childTm;
        ET_SendEventReturn(childTm, childId, &ETEntity::ET_getTransform);
        childTm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), Math::PI / 4.f);
        ET_SendEvent(childId, &ETEntity::ET_setTransform, childTm);
    }

    {
        Transform parentTm;
        ET_SendEventReturn(parentTm, parentId, &ETEntity::ET_getTransform);
        parentTm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), -Math::PI / 4.f);
        ET_SendEvent(parentId, &ETEntity::ET_setTransform, parentTm);
    }

    Transform tm;
    ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

    {
        Vec3 axis(0.f);
        float angle = 0.f;
        tm.quat.getAxisAngle(axis, angle);

        EXPECT_FLOAT_EQ(angle, 0.f);
    }

    {
        Transform parentTm;
        ET_SendEventReturn(parentTm, parentId, &ETEntity::ET_getTransform);
        parentTm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), Math::PI / 4.f);
        ET_SendEvent(parentId, &ETEntity::ET_setTransform, parentTm);
    }

    ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

    {
        Vec3 axis(0.f);
        float angle = 0.f;
        tm.quat.getAxisAngle(axis, angle);

        EXPECT_FLOAT_EQ(axis.x, 0.f);
        EXPECT_FLOAT_EQ(axis.y, 0.f);
        if(axis.z > 0.f) {
            EXPECT_FLOAT_EQ(axis.z, 1.f);
            EXPECT_FLOAT_EQ(angle, Math::PI / 2.f);
        } else {
            EXPECT_FLOAT_EQ(axis.z, -1.f);
            EXPECT_FLOAT_EQ(angle, -Math::PI / 2.f);
        }
    }
}

TEST_F(EntityTests, CheckLocalTransform) {
    auto parent = createVoidObject();
    auto child = createVoidObject();

    Transform tm;
    tm.pt = Vec3(1.f, 2.f, 3.f);

    parent->ET_setTransform(tm);
    parent->ET_addChild(child->getEntityId());

    {
        auto localTm = child->ET_getLocalTransform();
        EXPECT_FLOAT_EQ(localTm.pt.x, -1.f);
        EXPECT_FLOAT_EQ(localTm.pt.y, -2.f);
        EXPECT_FLOAT_EQ(localTm.pt.z, -3.f);
    }

    tm.pt += Vec3(1.f);
    tm.scale = Vec3(0.5f, 0.5f, 0.5f);
    parent->ET_setTransform(tm);

    {
        auto localTm = child->ET_getLocalTransform();
        EXPECT_FLOAT_EQ(localTm.pt.x, -1.f);
        EXPECT_FLOAT_EQ(localTm.pt.y, -2.f);
        EXPECT_FLOAT_EQ(localTm.pt.z, -3.f);

        EXPECT_FLOAT_EQ(localTm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(localTm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(localTm.scale.z, 1.f);
    }

    tm.pt = Vec3(0.f);
    tm.scale = Vec3(0.5f);
    child->ET_setLocalTransform(tm);

    {
        auto localTm = child->ET_getLocalTransform();
        EXPECT_FLOAT_EQ(localTm.pt.x, 0.f);
        EXPECT_FLOAT_EQ(localTm.pt.y, 0.f);
        EXPECT_FLOAT_EQ(localTm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(localTm.scale.x, 0.5f);
        EXPECT_FLOAT_EQ(localTm.scale.y, 0.5f);
        EXPECT_FLOAT_EQ(localTm.scale.z, 0.5f);

        auto currTm = child->ET_getTransform();
        EXPECT_FLOAT_EQ(currTm.pt.x, 2.f);
        EXPECT_FLOAT_EQ(currTm.pt.y, 3.f);
        EXPECT_FLOAT_EQ(currTm.pt.z, 4.f);

        EXPECT_FLOAT_EQ(currTm.scale.x, 0.25f);
        EXPECT_FLOAT_EQ(currTm.scale.y, 0.25f);
        EXPECT_FLOAT_EQ(currTm.scale.z, 0.25f);
    }
}

TEST_F(EntityTests, CheckScaleAffectLocalTransform) {
    auto parent = createVoidObject();
    auto child = createVoidObject();

    Transform tm;
    tm.pt = Vec3(1.f);
    tm.scale = Vec3(1.f);
    parent->ET_setTransform(tm);

    tm.pt = Vec3(2.f);
    tm.scale = Vec3(1.f);

    child->ET_setTransform(tm);

    parent->ET_addChild(child->getEntityId());

    tm.pt = Vec3(2.f);
    tm.scale = Vec3(0.5);
    parent->ET_setTransform(tm);

    {
        auto currTm = child->ET_getTransform();

        EXPECT_FLOAT_EQ(currTm.pt.x, 2.5f);
        EXPECT_FLOAT_EQ(currTm.pt.y, 2.5f);
        EXPECT_FLOAT_EQ(currTm.pt.z, 2.5f);

        auto currLocalTm = child->ET_getLocalTransform();

        EXPECT_FLOAT_EQ(currLocalTm.pt.x, 1.f);
        EXPECT_FLOAT_EQ(currLocalTm.pt.y, 1.f);
        EXPECT_FLOAT_EQ(currLocalTm.pt.z, 1.f);
    }

    tm.scale = Vec3(1.f);
    parent->ET_setTransform(tm);

    {
        auto currTm = child->ET_getTransform();

        EXPECT_FLOAT_EQ(currTm.pt.x, 3.f);
        EXPECT_FLOAT_EQ(currTm.pt.y, 3.f);
        EXPECT_FLOAT_EQ(currTm.pt.z, 3.f);

        auto currLocalTm = child->ET_getLocalTransform();

        EXPECT_FLOAT_EQ(currLocalTm.pt.x, 1.f);
        EXPECT_FLOAT_EQ(currLocalTm.pt.y, 1.f);
        EXPECT_FLOAT_EQ(currLocalTm.pt.z, 1.f);
    }

}

TEST_F(EntityTests, CheckRegisterEntityLogics) {
    EntityLogicsRegister logicsRegister("Test");
    logicsRegister.registerLogic<TestLogic>();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_registerLogics, logicsRegister);
    ASSERT_TRUE(res);

    const char* childDataStr = "";
    const char* logicsDataStr = "{ \"type\": \"TestLogic\", \"id\": 1, \"data\" : {} }";
    std::string resStr = StringFormat("{ \"children\" : [ %s ], \"logics\" : [ %s ] }", childDataStr, logicsDataStr);
    JSONNode node = JSONNode::ParseString(resStr.c_str());

    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntityFromJSON, node, "TestEnity");
    ASSERT_TRUE(entId.isValid());

    Transform tm;
    ET_SendEventReturn(tm, entId, &ETEntity::ET_getTransform);

    ASSERT_FLOAT_EQ(tm.pt.x, 0.0f);
    ASSERT_FLOAT_EQ(tm.pt.y, 0.0f);
    ASSERT_FLOAT_EQ(tm.pt.z, 0.0f);

    ASSERT_FLOAT_EQ(tm.scale.x, 1.f);
    ASSERT_FLOAT_EQ(tm.scale.y, 1.f);
    ASSERT_FLOAT_EQ(tm.scale.z, 1.f);

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
        ASSERT_FLOAT_EQ(xVal, 0.f);
    }
    {
        float yVal = 0.f;
        stream.read(yVal);
        ASSERT_FLOAT_EQ(yVal, 0.f);
    }
    {
        float zVal = 0.f;
        stream.read(zVal);
        ASSERT_FLOAT_EQ(zVal, 0.f);
    }
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

TEST_F(EntityTests, CheckOnlyUniqueLogicsRegistered) {
    JSONNode node;
    ET_SendEventReturn(node, &ETEntityManager::ET_getRegisteredLogics);
    ASSERT_TRUE(node);

    size_t allLogics = 0;
    std::set<std::string> uniqueLogics;

    for(auto& moduleNode : node) {
        for(auto& logicNode : moduleNode) {
            std::string logicName;
            logicNode.read(logicName);
            uniqueLogics.insert(logicName);
            ++allLogics;
        }
    }

    ASSERT_GE(allLogics, 0);
    ASSERT_EQ(allLogics, uniqueLogics.size());
}

TEST_F(EntityTests, CheckQueryLogicsByTypeId) {
    auto entity = createVoidObject();
    ET_SendEvent(&ETEntityManager::ET_addLogicToEntity, entity->getEntityId(), "UIAnimationSequence");
    ET_SendEvent(&ETEntityManager::ET_addLogicToEntity, entity->getEntityId(), "UIAnimationSequence");

    auto logics = GetEntityLogics<UIAnimationSequence>(entity->getEntityId());
    ASSERT_EQ(logics.size(), 2u);
    ASSERT_TRUE(logics[0]);
    ASSERT_TRUE(logics[1]);
}