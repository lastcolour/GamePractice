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
};

class ObjectWithResource {
public:

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<ObjectWithResource>("ObjectWithResource")) {
            classInfo->addResourceField("resource", &ObjectWithResource::setResource);
        }
    }

public:

    void setResource(const char* resName) {
        resource.reset(new std::string(resName));
    }

public:

    std::shared_ptr<std::string> resource;
};

class ObjectWithEnum {
public:

    enum class Numbers {
        Zero = 0,
        One = 1,
        Two = 2,
        Three = 3,
        Four = 4,
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8,
        Nine = 9
    };

public:

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto enumInfo = reflectCtx.enumInfo<Numbers>("Numbers")) {
            enumInfo->addValues<Numbers>({
                {"Zero", Numbers::Zero},
                {"One", Numbers::One},
                {"Two", Numbers::Two},
                {"Three", Numbers::Three},
                {"Four", Numbers::Four},
                {"Five", Numbers::Five},
                {"Six", Numbers::Six},
                {"Seven", Numbers::Seven},
                {"Eight", Numbers::Eight},
                {"Nine", Numbers::Nine}
            });
        }
        if(auto classInfo = reflectCtx.classInfo<ObjectWithEnum>("ObjectWithEnum")) {
            classInfo->addField("number", &ObjectWithEnum::number);
        }
    }

public:

    Numbers number;
};

class ObjectWitArray {
public:

    static void Reflect(ReflectContext& ctx) {
        if(auto classInfo = ctx.classInfo<ObjectWitArray>("ObjectWitArray")) {
            classInfo->addField("array", &ObjectWitArray::array);
        }
    }

public:

    std::vector<ObjectWithEnum> array;
};

class ObjectWithArrayOfVec3 {
public:

    static void Reflect(ReflectContext& ctx) {
        if(auto classInfo = ctx.classInfo<ObjectWithArrayOfVec3>("ObjectWithArrayOfVec3")) {
            classInfo->addField("array", &ObjectWithArrayOfVec3::array);
        }
    }

public:

    std::vector<Vec3> array;

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

        std::string typeVal;
        classNode.read("type", typeVal);
        ASSERT_STREQ(typeVal.c_str(), "class");

        auto dataNode = classNode.object("data");
        ASSERT_TRUE(dataNode);
        {
            std::string val;
            ASSERT_TRUE(dataNode.hasKey("boolF"));
            dataNode.read("boolF", val);
            ASSERT_STREQ(val.c_str(), "bool");
        }
        {
            std::string val;
            ASSERT_TRUE(dataNode.hasKey("intF"));
            dataNode.read("intF", val);
            ASSERT_STREQ(val.c_str(), "int");
        }
        {
            std::string val;
            ASSERT_TRUE(dataNode.hasKey("floatF"));
            dataNode.read("floatF", val);
            ASSERT_STREQ(val.c_str(), "float");
        }
        {
            std::string val;
            ASSERT_TRUE(dataNode.hasKey("stringF"));
            dataNode.read("stringF", val);
            ASSERT_STREQ(val.c_str(), "string");
        }
    }
    ASSERT_TRUE(node.hasKey("ObjectWithObjectEntity"));
    {
        auto classNode = node.object("ObjectWithObjectEntity");
        ASSERT_TRUE(classNode);
        {
            auto dataNode = classNode.object("data");
            ASSERT_TRUE(dataNode);
            ASSERT_TRUE(dataNode.hasKey("objectF"));
            std::string val;
            dataNode.read("objectF", val);
            ASSERT_STREQ(val.c_str(), "SimpleEntityLogic");
        }
    }
}

TEST_F(ReflectTests, TestBaseAndDeriveded) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<DerivedObject>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonStr = StringFormat("%s", SIMPLE_ENTITY_JSON_DATA);
    auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance(jsonNode);
    auto object = classInstance.acquire<DerivedObject>();
    ASSERT_TRUE(object);

    ChekcSimpleEntityReflect(object.get());

    int classReflected = 0;
    ET_SendEventReturn(classReflected, &ETClassInfoManager::ET_getRegisteredClassCount);
    ASSERT_EQ(classReflected, 2);
}

TEST_F(ReflectTests, TestResource) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithResource>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{ \"resource\" : \"ObjectWithResource\" }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance(jsonNode);
    auto object = classInstance.acquire<ObjectWithResource>();
    ASSERT_TRUE(object);

    ASSERT_TRUE(object->resource);
    ASSERT_STREQ(object->resource->c_str(), "ObjectWithResource");
}

TEST_F(ReflectTests, TestEnum) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithEnum>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{ \"number\" : \"Two\" }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance(jsonNode);
    auto object = classInstance.acquire<ObjectWithEnum>();
    ASSERT_TRUE(object);

    ASSERT_EQ(object->number, ObjectWithEnum::Numbers::Two);
}

TEST_F(ReflectTests, TestArray) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWitArray>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{\"array\": [{ \"number\" : \"One\" }, {\"number\" : \"Two\"}] }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance(jsonNode);
    auto object = classInstance.acquire<ObjectWitArray>();
    ASSERT_TRUE(object);

    ASSERT_EQ(object->array.size(), 2);
    ASSERT_EQ(object->array[0].number, ObjectWithEnum::Numbers::One);
    ASSERT_EQ(object->array[1].number, ObjectWithEnum::Numbers::Two);
}

TEST_F(ReflectTests, TestArrayOfVec3) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithArrayOfVec3>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{\"array\": [{ \"x\":1, \"y\":2, \"z\":3 }, { \"x\":4, \"y\":5, \"z\":6 }, { \"x\":7, \"y\":8, \"z\":9 }] }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance(jsonNode);
    auto object = classInstance.acquire<ObjectWithArrayOfVec3>();
    ASSERT_TRUE(object);

    ASSERT_EQ(object->array.size(), 3);
    ASSERT_FLOAT_EQ(object->array[0].x, 1.f);
    ASSERT_FLOAT_EQ(object->array[0].y, 2.f);
    ASSERT_FLOAT_EQ(object->array[0].z, 3.f);

    ASSERT_FLOAT_EQ(object->array[1].x, 4.f);
    ASSERT_FLOAT_EQ(object->array[1].y, 5.f);
    ASSERT_FLOAT_EQ(object->array[1].z, 6.f);

    ASSERT_FLOAT_EQ(object->array[2].x, 7.f);
    ASSERT_FLOAT_EQ(object->array[2].y, 8.f);
    ASSERT_FLOAT_EQ(object->array[2].z, 9.f);
}

TEST_F(ReflectTests, TestDerivedReflection) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<DerivedObject>());

    JSONNode node;
    ET_SendEvent(&ETClassInfoManager::ET_makeReflectModel, node);

    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 2u);

    ASSERT_TRUE(node.hasKey("DerivedObject"));
    ASSERT_TRUE(node.hasKey("SimpleEntityLogic"));

    auto classNode = node.object("DerivedObject");
    ASSERT_TRUE(classNode);

    ASSERT_TRUE(classNode.hasKey("base"));
}

TEST_F(ReflectTests, TestEntityReference) {
    ASSERT_TRUE(false);
}