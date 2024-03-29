#include "ReflectTests.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Core/JSONNode.hpp"
#include "Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "EntityRegistry.hpp"
#include "Reflect/ClassInfoManager.hpp"
#include "Reflect/PolymorphPtr.hpp"
#include "Core/MemoryStream.hpp"

namespace {

class CounterObject {
public:

    static int ConstructorCallNum;
    static int DestructorCallNum;

public:

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<CounterObject>("CounterObject")) {
            (void)classInfo;
        }
    }

public:

    CounterObject() {
        ConstructorCallNum += 1;
    }

    ~CounterObject() {
        DestructorCallNum += 1;
    }

private:

    CounterObject(const CounterObject&);
};

int CounterObject::ConstructorCallNum = 0;
int CounterObject::DestructorCallNum = 0;

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

class ObjectWithStringArray {
public:

    static void Reflect(ReflectContext& ctx) {
        if(auto classInfo = ctx.classInfo<ObjectWithStringArray>("ObjectWithStringArray")) {
            classInfo->addField("array", &ObjectWithStringArray::array);
        }
    }

public:

    std::vector<std::string> array;
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

class ObjectWithEntity : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx) {
        if(auto classInfo = ctx.classInfo<ObjectWithEntity>("ObjectWithEntity")) {
            classInfo->addField("entity", &ObjectWithEntity::entityId);
        }
    }

public:

    // EntityLogic
    void init() override {}
    void deinit() override {}

public:

    EntityId entityId;
};

class ObjectWithPolymoprhMember {
public:

    static void Reflect(ReflectContext& ctx) {
        if(auto classInfo = ctx.classInfo<ObjectWithPolymoprhMember>("ObjectWithPolymoprhMember")) {
            classInfo->addField("logic", &ObjectWithPolymoprhMember::logic);
        }
    }

public:

    Reflect::PolymorphPtr<SimpleEntityLogic> logic;
};

} // namespace

void ReflectTests::TearDown() {
    GetEnv()->GetClassInfoManager()->reset();
}

TEST_F(ReflectTests, TestSimpleLogic) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<SimpleEntityLogic>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonStr = StringFormat("%s", SIMPLE_ENTITY_JSON_DATA);
    auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    ASSERT_EQ(classInstance.getInstanceTypeId(), classInfo->getIntanceTypeId());

    auto object = classInstance.castTo<SimpleEntityLogic>();
    ASSERT_TRUE(object);

    ChekcSimpleEntityReflect(object);

    ASSERT_EQ(GetEnv()->GetClassInfoManager()->getRegisteredClassCount(), 1);
}

TEST_F(ReflectTests, TestReCreateInPlace) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<CounterObject>());

    const int StartConstructVal = CounterObject::ConstructorCallNum;
    const int StartDescructVal = CounterObject::DestructorCallNum;

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    auto instance = classInfo->createInstance();

    EXPECT_EQ(CounterObject::ConstructorCallNum, StartConstructVal + 1);
    EXPECT_EQ(CounterObject::DestructorCallNum, StartDescructVal);

    instance.reCreate();

    EXPECT_EQ(CounterObject::ConstructorCallNum, StartConstructVal + 2);
    EXPECT_EQ(CounterObject::DestructorCallNum, StartDescructVal + 1);
}

TEST_F(ReflectTests, TestObjectWithObject) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithObjectEntity>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonStr = StringFormat("{ \"objectF\" : %s }", SIMPLE_ENTITY_JSON_DATA);
    auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    auto object = classInstance.castTo<ObjectWithObjectEntity>();
    ASSERT_TRUE(object);

    ChekcSimpleEntityReflect(&object->objectF);

    ASSERT_EQ(GetEnv()->GetClassInfoManager()->getRegisteredClassCount(), 2);
}

TEST_F(ReflectTests, TestReflectModel) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithObjectEntity>());

    JSONNode node;
    GetEnv()->GetClassInfoManager()->makeReflectModel(node);

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

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    auto object = classInstance.castTo<DerivedObject>();
    ASSERT_TRUE(object);

    ChekcSimpleEntityReflect(object);

    ASSERT_EQ(GetEnv()->GetClassInfoManager()->getRegisteredClassCount(), 2);
}

TEST_F(ReflectTests, TestEnum) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithEnum>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{ \"number\" : \"Two\" }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    auto object = classInstance.castTo<ObjectWithEnum>();
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

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    auto object = classInstance.castTo<ObjectWitArray>();
    ASSERT_TRUE(object);

    ASSERT_EQ(object->array.size(), 2u);
    ASSERT_EQ(object->array[0].number, ObjectWithEnum::Numbers::One);
    ASSERT_EQ(object->array[1].number, ObjectWithEnum::Numbers::Two);
}

TEST_F(ReflectTests, TestObjectWithStringArray) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithStringArray>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{\"array\": [\"one\", \"two\"] }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    auto object = static_cast<ObjectWithStringArray*>(classInstance.get());
    ASSERT_TRUE(object);

    ASSERT_EQ(object->array.size(), 2u);
    ASSERT_STREQ(object->array[0].c_str(), "one");
    ASSERT_STREQ(object->array[1].c_str(), "two");

    Reflect::ClassValueId valueId = 1;
    ASSERT_TRUE(classInstance.addValueArrayElement(valueId));

    Memory::MemoryStream stream;
    stream.openForWrite();

    ASSERT_TRUE(classInstance.writeValueTo(ctx, Reflect::AllClassValuesId, stream));

    stream.reopenForRead();

    int size = 0;
    stream.read(size);
    ASSERT_EQ(size, 3);
    {
        std::string first;
        stream.read(first);
        ASSERT_STREQ(first.c_str(), "one");
    }
    {
        std::string second;
        stream.read(second);
        ASSERT_STREQ(second.c_str(), "two");
    }
    {
        std::string third;
        stream.read(third);
        ASSERT_STREQ(third.c_str(), "");
    }
}

TEST_F(ReflectTests, TestArrayOfVec3) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithArrayOfVec3>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{\"array\": [{ \"x\":1, \"y\":2, \"z\":3 }, { \"x\":4, \"y\":5, \"z\":6 }, { \"x\":7, \"y\":8, \"z\":9 }] }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));
    auto object = classInstance.castTo<ObjectWithArrayOfVec3>();
    ASSERT_TRUE(object);

    ASSERT_EQ(object->array.size(), 3u);
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
    GetEnv()->GetClassInfoManager()->makeReflectModel(node);

    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 2u);

    ASSERT_TRUE(node.hasKey("DerivedObject"));
    ASSERT_TRUE(node.hasKey("SimpleEntityLogic"));

    auto classNode = node.object("DerivedObject");
    ASSERT_TRUE(classNode);

    ASSERT_TRUE(classNode.hasKey("base"));
}

TEST_F(ReflectTests, TestReadValuesSimpleObject) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<SimpleEntityLogic>());

    auto jsonStr = StringFormat("%s", SIMPLE_ENTITY_JSON_DATA);
    auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
    ASSERT_TRUE(jsonNode);

    auto classInfo =  reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    Reflect::SerializeContext ctx;

    auto instance = classInfo->createInstance();
    ASSERT_TRUE(instance.readAllValuesFrom(ctx, jsonNode));

    Memory::MemoryStream stream;
    stream.openForWrite();

    ASSERT_TRUE(instance.writeAllValuesTo(ctx, stream));

    auto buffer = stream.flushToBuffer();
    ASSERT_TRUE(buffer);

    stream.close();

    stream.openForRead(buffer);
    {
        bool val = false;
        stream.read(val);
        ASSERT_EQ(val, true);
    }
    {
        int val = 0;
        stream.read(val);
        ASSERT_EQ(val, 1);
    }
    {
        float val = 1.0f;
        stream.read(val);
        ASSERT_FLOAT_EQ(val, 1.0f);
    }
    {
        std::string val;
        stream.read(val);
        ASSERT_STREQ(val.c_str(), "1");
    }
}

TEST_F(ReflectTests, TestWriteClassValueOfSimpleEntity) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<SimpleEntityLogic>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto instance = classInfo->createInstance();
    ASSERT_TRUE(instance.get());

    auto logicPtr = static_cast<SimpleEntityLogic*>(instance.get());

    bool boolVal = true;
    int intVal = 1;
    float floatVal = 1.f;
    std::string strVal = "1";

    Memory::MemoryStream stream;
    stream.openForWrite();

    stream.write(boolVal);
    stream.write(intVal);
    stream.write(floatVal);
    stream.write(strVal.c_str());

    stream.reopenForRead();

    Reflect::SerializeContext ctx;

    ASSERT_TRUE(instance.readValueFrom(ctx, 1, stream));
    ASSERT_TRUE(instance.readValueFrom(ctx, 2, stream));
    ASSERT_TRUE(instance.readValueFrom(ctx, 3, stream));
    ASSERT_TRUE(instance.readValueFrom(ctx, 4, stream));

    ASSERT_EQ(logicPtr->boolF, boolVal);
    ASSERT_EQ(logicPtr->intF, intVal);
    ASSERT_FLOAT_EQ(logicPtr->floatF, floatVal);
    ASSERT_STREQ(logicPtr->stringF.c_str(), strVal.c_str());
}

TEST_F(ReflectTests, TestReadClassValueOfSimpleEntity) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<SimpleEntityLogic>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto instance = classInfo->createInstance();
    ASSERT_TRUE(instance.get());

    auto logicPtr = static_cast<SimpleEntityLogic*>(instance.get());
    logicPtr->boolF = true;
    logicPtr->intF = 1;
    logicPtr->floatF = 1.f;
    logicPtr->stringF = "1";

    Memory::MemoryStream stream;

    stream.openForWrite();

    Reflect::SerializeContext ctx;

    ASSERT_TRUE(instance.writeValueTo(ctx, 1, stream));
    ASSERT_TRUE(instance.writeValueTo(ctx, 2, stream));
    ASSERT_TRUE(instance.writeValueTo(ctx, 3, stream));
    ASSERT_TRUE(instance.writeValueTo(ctx, 4, stream));

    stream.reopenForRead();

    {
        bool boolVal = false;
        stream.read(boolVal);
        ASSERT_EQ(boolVal, logicPtr->boolF);
    }
    {
        int intVal = 0;
        stream.read(intVal);
        ASSERT_EQ(intVal, logicPtr->intF);
    }
    {
        float floatVal = 0.f;
        stream.read(floatVal);
        ASSERT_FLOAT_EQ(floatVal, logicPtr->floatF);
    }
    {
        std::string stringVal;
        stream.read(stringVal);
        ASSERT_STREQ(stringVal.c_str(), logicPtr->stringF.c_str());
    }
}

TEST_F(ReflectTests, TestReadWriteClassValueOfObjectWithObject) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithObjectEntity>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto instance = classInfo->createInstance();
    ASSERT_TRUE(instance.get());

    auto objectPtr = static_cast<ObjectWithObjectEntity*>(instance.get());

    objectPtr->objectF.floatF = 1.f;

    Memory::MemoryStream stream;
    stream.openForWrite();

    Reflect::SerializeContext ctx;

    ASSERT_TRUE(instance.writeValueTo(ctx, 3, stream));

    stream.reopenForRead();

    float val = 0.f;
    stream.read(val);

    ASSERT_FLOAT_EQ(objectPtr->objectF.floatF, val);

    stream.reopenForWrite();
    stream.write(2.f);

    stream.reopenForRead();

    instance.readValueFrom(ctx, 3, stream);

    ASSERT_FLOAT_EQ(objectPtr->objectF.floatF, 2.f);
}

TEST_F(ReflectTests, TestReadWriteClassValueOfObjectWitArray) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWitArray>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto instance = classInfo->createInstance();
    auto objectPtr = static_cast<ObjectWitArray*>(instance.get());
    ASSERT_TRUE(objectPtr);

    ASSERT_TRUE(objectPtr->array.empty());

    Memory::MemoryStream stream;
    stream.openForWrite();

    stream.write(3);
    stream.write(static_cast<int>(ObjectWithEnum::Numbers::One));
    stream.write(static_cast<int>(ObjectWithEnum::Numbers::Two));
    stream.write(static_cast<int>(ObjectWithEnum::Numbers::Three));

    stream.reopenForRead();

    Reflect::SerializeContext ctx;
    ASSERT_TRUE(instance.readValueFrom(ctx, 1, stream));

    ASSERT_EQ(objectPtr->array.size(), 3u);

    ASSERT_EQ(objectPtr->array[0].number, ObjectWithEnum::Numbers::One);
    ASSERT_EQ(objectPtr->array[1].number, ObjectWithEnum::Numbers::Two);
    ASSERT_EQ(objectPtr->array[2].number, ObjectWithEnum::Numbers::Three);

    objectPtr->array.clear();
    objectPtr->array.emplace_back();
    objectPtr->array.back().number = ObjectWithEnum::Numbers::Four;
    objectPtr->array.emplace_back();
    objectPtr->array.back().number = ObjectWithEnum::Numbers::Five;

    stream.reopenForWrite();

    instance.writeValueTo(ctx, 1, stream);

    stream.reopenForRead();

    int size = 0;
    stream.read(size);
    ASSERT_EQ(size, 2);
    {
        int firstVal = 0;
        stream.read(firstVal);
        ASSERT_EQ(firstVal, static_cast<int>(ObjectWithEnum::Numbers::Four));
    }
    {
        int secondVal = 0;
        stream.read(secondVal);
        ASSERT_EQ(secondVal, static_cast<int>(ObjectWithEnum::Numbers::Five));
    }
}

TEST_F(ReflectTests, CheckAddArrayElemet) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWitArray>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto instance = classInfo->createInstance();
    auto objectPtr = static_cast<ObjectWitArray*>(instance.get());
    ASSERT_TRUE(objectPtr);

    ASSERT_TRUE(objectPtr->array.empty());

    Reflect::ClassValueId valueId = 1;
    ASSERT_TRUE(instance.addValueArrayElement(valueId));
    ASSERT_FALSE(objectPtr->array.empty());

    Memory::MemoryStream stream;
    stream.openForWrite();

    Reflect::SerializeContext ctx;
    ASSERT_TRUE(instance.writeValueTo(ctx, 1, stream));

    stream.reopenForRead();

    int size = 0;
    stream.read(size);

    ASSERT_EQ(size, 1);
}

TEST_F(ReflectTests, TestEntityReference) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithEntity>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    ObjectWithEntity* logicPtr = nullptr;
    EntityLogicId logicId = InvalidEntityLogicId;

    EntityRegistry registy;
    auto parentEntity = registy.createEntity("Parent");

    auto logicInstance = classInfo->createInstance();
    logicPtr = static_cast<ObjectWithEntity*>(logicInstance.get());
    logicId = parentEntity->addLogic(std::move(logicInstance));
    ASSERT_NE(logicId, InvalidEntityLogicId);

    EntityChildId childId = 1;
    auto childEntity = registy.createEntity("Child");
    parentEntity->addChildEntityWithId(childId, *childEntity);

    Memory::MemoryStream stream;
    stream.openForWrite();
    stream.write(1);
    stream.write(childId);

    stream.reopenForRead();

    parentEntity->writeLogicData(logicId, Reflect::AllClassValuesId, stream);

    ASSERT_EQ(logicPtr->entityId, childEntity->getEntityId());
}

TEST_F(ReflectTests, TestSelfReferencing) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithEntity>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    ObjectWithEntity* logicPtr = nullptr;
    EntityLogicId logicId = InvalidEntityLogicId;

    EntityRegistry registy;
    auto parentEntity = registy.createEntity("Parent");

    auto logicInstance = classInfo->createInstance();
    logicPtr = static_cast<ObjectWithEntity*>(logicInstance.get());
    
    {
        auto jsonNode = JSONNode::ParseString("{ \"entity\": [0] }");
        ASSERT_TRUE(jsonNode);

        Reflect::SerializeContext ctx;
        ctx.entityId = parentEntity->getEntityId();
        logicInstance.readAllValuesFrom(ctx, jsonNode);

        EXPECT_EQ(logicPtr->entityId, parentEntity->getEntityId());
    }

    {
        auto jsonNode = JSONNode::ParseString("{ \"entity\": [] }");
        ASSERT_TRUE(jsonNode);

        Reflect::SerializeContext ctx;
        ctx.entityId = parentEntity->getEntityId();
        logicInstance.readAllValuesFrom(ctx, jsonNode);

        EXPECT_EQ(logicPtr->entityId, InvalidEntityId);
    }

    logicId = parentEntity->addLogic(std::move(logicInstance));
    ASSERT_NE(logicId, InvalidEntityLogicId);

    Memory::MemoryStream stream;
    stream.openForWrite();
    stream.write(1);
    stream.write(0);

    stream.reopenForRead();

    parentEntity->writeLogicData(logicId, Reflect::AllClassValuesId, stream);

    ASSERT_EQ(logicPtr->entityId, parentEntity->getEntityId());

    stream.reopenForWrite();

    parentEntity->readLogicData(logicId, Reflect::AllClassValuesId, stream);

    stream.reopenForRead();

    {
        int val = 0;
        stream.read(val);
        EXPECT_EQ(val, 1);

        val = -1;
        stream.read(val);
        EXPECT_EQ(val, 0);
    }
}

TEST_F(ReflectTests, CheckLongEntityReference) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithEntity>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    ObjectWithEntity* logicPtr = nullptr;
    EntityLogicId logicId = InvalidEntityLogicId;

    EntityRegistry registy;
    auto parentEntity = registy.createEntity("Parent");

    auto logicInstance = classInfo->createInstance();
    logicPtr = static_cast<ObjectWithEntity*>(logicInstance.get());
    logicId = parentEntity->addLogic(std::move(logicInstance));
    ASSERT_NE(logicId, InvalidEntityLogicId);

    EntityChildId firstChildId = 1;
    auto firstChildEntity = registy.createEntity("FirstChild");
    parentEntity->addChildEntityWithId(firstChildId, *firstChildEntity);

    EntityChildId secondChildId = 3;
    auto secondChildEntity = registy.createEntity("SecondChild");
    firstChildEntity->addChildEntityWithId(secondChildId, *secondChildEntity);

    Memory::MemoryStream stream;
    stream.openForWrite();
    stream.write(2);
    stream.write(firstChildId);
    stream.write(secondChildId);

    stream.reopenForRead();

    parentEntity->writeLogicData(logicId, Reflect::AllClassValuesId, stream);

    ASSERT_EQ(logicPtr->entityId, secondChildEntity->getEntityId());
}

TEST_F(ReflectTests, CheckPolymorphObjects) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithPolymoprhMember>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto instance = classInfo->createInstance();
    auto objectPtr = static_cast<ObjectWithPolymoprhMember*>(instance.get());
    ASSERT_TRUE(objectPtr);

    {
        auto jsonStr = StringFormat("{\"logic\" : {\"type\": \"SimpleEntityLogic\", \"data\": %s }}", SIMPLE_ENTITY_JSON_DATA);
        auto jsonNode = JSONNode::ParseString(jsonStr.c_str());
        ASSERT_TRUE(jsonNode);

        Reflect::SerializeContext ctx;
        ASSERT_TRUE(instance.readAllValuesFrom(ctx, jsonNode));

        ASSERT_TRUE(objectPtr->logic.get());
    }

    objectPtr->logic->intF = 1024;

    {
        JSONNode node;

        Reflect::SerializeContext ctx;
        ASSERT_TRUE(instance.writeAllValuesTo(ctx, node));

        JSONNode logicNode = node.object("logic");
        ASSERT_TRUE(logicNode);

        std::string typeName;
        logicNode.read("type", typeName);

        ASSERT_STREQ(typeName.c_str(), "SimpleEntityLogic");

        auto dataNode = logicNode.object("data");

        int val = 0;
        dataNode.read("intF", val);

        ASSERT_EQ(val, 1024);
    }
}