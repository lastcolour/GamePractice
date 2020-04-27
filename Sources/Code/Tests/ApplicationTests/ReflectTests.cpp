#include "ReflectTests.hpp"
#include <Reflect/ClassInfo.hpp>
#include <Reflect/ReflectContext.hpp>
#include <Core/JSONNode.hpp>
#include <Core/StringFormat.hpp>

namespace {

class SimpleEntityLogic {
public:

    SimpleEntityLogic() :
        boolF(false),
        intF(0),
        floatF(0.f),
        stringF() {}

    static void reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<SimpleEntityLogic>("SimpleEntityLogic")) {
            classInfo->addField("boolF", &SimpleEntityLogic::boolF);
            classInfo->addField("intF", &SimpleEntityLogic::intF);
            classInfo->addField("floatF", &SimpleEntityLogic::floatF);
            classInfo->addField("stringF", &SimpleEntityLogic::stringF);
        }
    }

public:

    bool boolF;
    int intF;
    float floatF;
    std::string stringF;
};

const char* SIMPLE_ENTITY_JSON_DATA = "{ \"boolF\" : true, \"intF\" : 1, \"floatF\" : 1.0, \"stringF\" : \"1\" }";

void ChekcSimpleEntityReflect(SimpleEntityLogic* object) {
    ASSERT_EQ(object->boolF, true);
    ASSERT_EQ(object->intF, 1);
    ASSERT_FLOAT_EQ(object->floatF, 1.0f);
    ASSERT_STREQ(object->stringF.c_str(), "1");
}

class ObjectWithObjectEntity {
public:

    static void reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<ObjectWithObjectEntity>("ObjectWithObjectEntity")) {
            classInfo->addField("objectF", &ObjectWithObjectEntity::objectF);
        }
    }


public:

    SimpleEntityLogic objectF;
};

} // namespace

TEST_F(ReflectTests, TestSimpleLogic) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<SimpleEntityLogic>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonStr = StringFormat("%s", SIMPLE_ENTITY_JSON_DATA);
    auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
    ASSERT_TRUE(jsonNode);

    auto classIntance = classInfo->createInstance(jsonNode);
    auto object = classIntance.acquire<SimpleEntityLogic>();
    ASSERT_TRUE(object);

    ChekcSimpleEntityReflect(object.get());
}

TEST_F(ReflectTests, TestObjectWithObject) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithObjectEntity>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonStr = StringFormat("{ \"objectF\" : %s }", SIMPLE_ENTITY_JSON_DATA);
    auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance(jsonNode);
    auto object = classInstance.acquire<ObjectWithObjectEntity>();
    ASSERT_TRUE(object);

    ChekcSimpleEntityReflect(&object->objectF);
}