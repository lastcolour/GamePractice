#include "CoreTests.hpp"

#include "Core/TypeId.hpp"
#include "Core/StringFormat.hpp"
#include "Core/ETPrimitives.hpp"
#include "Environment.hpp"

namespace {

class ET_TestInterface {
public:
    virtual ~ET_TestInterface() = default;
    virtual void ET_onEvent() = 0;
    virtual void ET_OnParamEvent(int param) = 0;
    virtual int ET_OnReturnParamValue() = 0;
};

class TestETNode : public ETNode<ET_TestInterface> {
public:

    TestETNode() : eventRecieved(0) {}
    virtual ~TestETNode() = default;

    void connect(EntityId id) {
        ETNode<ET_TestInterface>::connect(id);
    }

    void disconnect() {
        ETNode<ET_TestInterface>::disconnect();
    }

    void ET_onEvent() override {
        ++eventRecieved;
    }

    void ET_OnParamEvent(int param) override {
        ++eventRecieved;
    }

    int ET_OnReturnParamValue() override {
        ++eventRecieved;
        return eventRecieved;
    }

public:

    int eventRecieved;
};

} // namespace

TEST_F(CoreTests, CheckSameTypeID) {
    auto intTypeId1 = GetTypeId<int>();
    auto intTypeId2 = GetTypeId<int>();
    ASSERT_EQ(intTypeId1, intTypeId2);
}

TEST_F(CoreTests, CheckSameTypeIDViaTypeDef) {
    typedef float TypedefFloat;
    auto floatTypeId1 = GetTypeId<float>();
    auto floatTypeId2 = GetTypeId<TypedefFloat>();
    ASSERT_EQ(floatTypeId1, floatTypeId2);
}

TEST_F(CoreTests, CheckDifferentTypes) {
    auto dobuleTypeId = GetTypeId<double>();
    auto charTypeId = GetTypeId<char>();
    ASSERT_NE(dobuleTypeId, charTypeId);
}

TEST_F(CoreTests, CheckNormalFormats) {
    const std::string s = "1";
    std::string str = StringFormat("%i %.1f %s %s", 1, 1.0f, "1", s);
    ASSERT_STREQ(str.c_str(), "1 1.0 1 1");
}

TEST_F(CoreTests, CheckVoidFormats) {
    const std::string testStr = "test_str";
    std::string resStr = StringFormat(testStr);
    ASSERT_EQ(resStr, testStr);
}

TEST_F(CoreTests, CheckBaseEventLogicToOneListener) {
    TestETNode node;
    EntityId entId;
    entId.setRawId(1);
    node.connect(entId);
    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node.eventRecieved, 1);

    node.disconnect();
    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node.eventRecieved, 1);
}

TEST_F(CoreTests, CheckBaseEventLogicToTwoListener) {
    EntityId entId;
    entId.setRawId(1);

    TestETNode node1;
    node1.connect(entId);

    TestETNode node2;
    node2.connect(entId);
    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node1.eventRecieved, 1);
    ASSERT_EQ(node2.eventRecieved, 1);

    node1.disconnect();
    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node1.eventRecieved, 1);
    ASSERT_EQ(node2.eventRecieved, 2);

    node2.disconnect();

    ASSERT_EQ(node1.eventRecieved, 1);
    ASSERT_EQ(node2.eventRecieved, 2);
}

TEST_F(CoreTests, CheckAutoUnsubscribeAfterDelete) {
    EntityId entId;
    entId.setRawId(1);

    std::unique_ptr<TestETNode> nodePtr(new TestETNode);
    nodePtr->connect(entId);

    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);
    ASSERT_EQ(nodePtr->eventRecieved, 1);

    nodePtr.reset();

    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);
}

TEST_F(CoreTests, CheckDoubleConnect) {
    TestETNode node;
    EntityId entId;
    entId.setRawId(1);
    node.connect(entId);
    node.connect(entId);
    ET_SendEvent(entId, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node.eventRecieved, 1);
}

TEST_F(CoreTests, CheclWithAruments) {
    TestETNode node;
    EntityId entId;
    entId.setRawId(1);
    node.connect(entId);
    ET_SendEvent(entId, &ET_TestInterface::ET_OnParamEvent, 3);

    ASSERT_EQ(node.eventRecieved, 1);
}

TEST_F(CoreTests, CheckWithReturnValue) {
    TestETNode node;
    EntityId entId;
    entId.setRawId(1);
    node.connect(entId);

    int retVal = 0;
    ET_SendEventReturn(retVal, entId, &ET_TestInterface::ET_OnReturnParamValue);
    
    ASSERT_EQ(node.eventRecieved, retVal);
}