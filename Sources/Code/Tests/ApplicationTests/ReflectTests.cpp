#include "ReflectTests.hpp"
#include <Reflect/ClassInfo.hpp>
#include <Reflect/ReflectContext.hpp>
#include <Core/JSONNode.hpp>
#include <Core/StringFormat.hpp>
#include <Core/ETPrimitives.hpp>
#include <Reflect/ETReflectInterfaces.hpp>

namespace {

class SimpleEntityLogic {
public:

    SimpleEntityLogic() :
        boolF(false),
        intF(0),
        floatF(0.f),
        stringF() {}

    static void Reflect(ReflectContext& reflectCtx) {
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

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<ObjectWithObjectEntity>("ObjectWithObjectEntity")) {
            classInfo->addField("objectF", &ObjectWithObjectEntity::objectF);
        }
    }

public:

    SimpleEntityLogic objectF;
};

class DerivedObject : public SimpleEntityLogic {
public:

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<DerivedObject>("DerivedObject")) {
            classInfo->addBaseClass<SimpleEntityLogic>();
        }
    }

public:

};

} // namespace

void ReflectTests::TearDown() {
    ET_SendEvent(&ETClassInfoManager::ET_reset);
}

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

    int classReflected = 0;
    ET_SendEventReturn(classReflected, &ETClassInfoManager::ET_getRegisteredClassCount);
    ASSERT_EQ(classReflected, 1);
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

    int classReflected = 0;
    ET_SendEventReturn(classReflected, &ETClassInfoManager::ET_getRegisteredClassCount);
    ASSERT_EQ(classReflected, 2);
}

TEST_F(ReflectTests, TestReflectModel) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithObjectEntity>());

    JSONNode node;
    ET_SendEvent(&ETClassInfoManager::ET_makeReflectModel, node);

    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 2u);

    ASSERT_TRUE(node.hasKey("SimpleEntityLogic"));
    {
        auto classNode = node.object("SimpleEntityLogic");
        ASSERT_TRUE(classNode);
        {
            std::string val;
            ASSERT_TRUE(classNode.hasKey("boolF"));
            classNode.read("boolF", val);
            ASSERT_STREQ(val.c_str(), "bool");
        }
        {
            std::string val;
            ASSERT_TRUE(classNode.hasKey("intF"));
            classNode.read("intF", val);
            ASSERT_STREQ(val.c_str(), "int");
        }
        {
            std::string val;
            ASSERT_TRUE(classNode.hasKey("floatF"));
            classNode.read("floatF", val);
            ASSERT_STREQ(val.c_str(), "float");
        }
        {
            std::string val;
            ASSERT_TRUE(classNode.hasKey("stringF"));
            classNode.read("stringF", val);
            ASSERT_STREQ(val.c_str(), "string");
        }
    }
    ASSERT_TRUE(node.hasKey("ObjectWithObjectEntity"));
    {
        auto classNode = node.object("ObjectWithObjectEntity");
        ASSERT_TRUE(classNode);
        {
            std::string val;
            ASSERT_TRUE(classNode.hasKey("objectF"));
            classNode.read("objectF", val);
            ASSERT_STREQ(val.c_str(), "SimpleEntityLogic");
        }
    }
}

TEST_F(ReflectTests, TestBaseAndDeriveded) {
    ASSERT_TRUE(false);
}

TEST_F(ReflectTests, TestResource) {
    ASSERT_TRUE(false);
}

TEST_F(ReflectTests, TestEnum) {
    ASSERT_TRUE(false);
}

TEST_F(ReflectTests, TestEntityReference) {
    ASSERT_TRUE(false);
}

TEST_F(ReflectTests, TestArray) {
    ASSERT_TRUE(false);
}