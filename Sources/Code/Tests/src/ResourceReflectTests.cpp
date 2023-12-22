#include "ResourceReflectTests.hpp"
#include "Reflect/Resource.hpp"
#include "Core/JSONNode.hpp"
#include "Reflect/ClassInfoManager.hpp"

#include <string>

namespace {

class TestObject {
public:

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<TestObject>("TestObject")) {
            classInfo->addField("val", &TestObject::val);
        }
    }

public:

    int val;
};

class ObjectWithResources {
public:

    struct SimpleResourceDescr {
        using RuntimeType = std::string;
        using StorageType = std::string;
        const char* TypeName = "Simple";
    };

    struct ConvertedResourceDescr {
        using StorageType = int;
        using RuntimeType = std::string;
        const char* TypeName = "Covertable";

        static void Convert(const StorageType& from, RuntimeType& to) {
            to = std::to_string(from);
        }

        static void Convert(const RuntimeType& from, StorageType& to) {
            to = std::stoi(from);
        }
    };

    struct ObjectResourceDescr {
        using StorageType = TestObject;
        using RuntimeType = TestObject;
        const char* TypeName = "Object";
    };

    static void Reflect(ReflectContext& reflectCtx) {
        if(auto classInfo = reflectCtx.classInfo<ObjectWithResources>("ObjectWithResources")) {
            classInfo->addField("simple", &ObjectWithResources::simple);
            classInfo->addField("converted", &ObjectWithResources::converted);
            classInfo->addField("object", &ObjectWithResources::object);
        }
    }

public:

    Reflect::Resource<SimpleResourceDescr> simple;
    Reflect::Resource<ConvertedResourceDescr> converted;
    Reflect::Resource<ObjectResourceDescr> object;
};

} // namespace

void ResourceReflectTests::TearDown() {
    GetEnv()->GetClassInfoManager()->reset();
}

TEST_F(ResourceReflectTests, TestTypeChecks) {
    using namespace Reflect;

    static_assert(ClassValueType::Resource == GetClassValueType<Resource<ObjectWithResources::SimpleResourceDescr>>());
    static_assert(ClassValueType::Resource == GetClassValueType<Resource<ObjectWithResources::ConvertedResourceDescr>>());
    static_assert(ClassValueType::Resource == GetClassValueType<Resource<ObjectWithResources::ObjectResourceDescr>>());
}

TEST_F(ResourceReflectTests, TestResource) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithResources>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    auto jsonNode = JSONNode::ParseString("{ \"simple\" : \"abc\", \"converted\" : 123, \"object\" : { \"val\" : 1 } }");
    ASSERT_TRUE(jsonNode);

    auto classInstance = classInfo->createInstance();
    Reflect::SerializeContext ctx;
    ASSERT_TRUE(classInstance.readAllValuesFrom(ctx, jsonNode));

    auto data = classInstance.castTo<ObjectWithResources>();
    ASSERT_TRUE(data);

    EXPECT_TRUE(data->simple.isSet());
    EXPECT_TRUE(data->converted.isSet());
    EXPECT_TRUE(data->object.isSet());

    EXPECT_STREQ(data->simple.get().c_str(), "abc");
    EXPECT_STREQ(data->converted.get().c_str(), "123");
    EXPECT_EQ(data->object.get().val, 1);

    data->simple.reset();
    EXPECT_FALSE(data->simple.isSet());

    jsonNode = JSONNode();
    classInstance.writeAllValuesTo(ctx, jsonNode);

    EXPECT_TRUE(jsonNode.hasKey("simple"));
    EXPECT_TRUE(jsonNode.isNull("simple"));
}

TEST_F(ResourceReflectTests, TestResourceMemoryStream) {
    ReflectContext reflectCtx;
    ASSERT_TRUE(reflectCtx.reflect<ObjectWithResources>());

    auto classInfo = reflectCtx.getRegisteredClassInfo();
    ASSERT_TRUE(classInfo);

    Memory::MemoryStream stream;
    stream.openForWrite();

    {
        auto classInstance = classInfo->createInstance();
        auto data = classInstance.castTo<ObjectWithResources>();

        data->simple.set("abc");
        data->converted.set("123");
        data->object.set(TestObject{1});

        Reflect::SerializeContext ctx;
        ASSERT_TRUE(classInstance.writeAllValuesTo(ctx, stream));
        ASSERT_TRUE(data);
    }

    stream.reopenForRead();

    auto classInstance = classInfo->createInstance();

    {
        Reflect::SerializeContext ctx;
        classInstance.readAllValuesFrom(ctx, stream);
    }

    auto data = classInstance.castTo<ObjectWithResources>();

    EXPECT_TRUE(data->simple.isSet());
    EXPECT_TRUE(data->converted.isSet());
    EXPECT_TRUE(data->object.isSet());

    EXPECT_STREQ(data->simple.get().c_str(), "abc");
    EXPECT_STREQ(data->converted.get().c_str(), "123");
    EXPECT_EQ(data->object.get().val, 1);
}