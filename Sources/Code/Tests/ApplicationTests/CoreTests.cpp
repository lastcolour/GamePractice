#include "CoreTests.hpp"

#include "Core/TypeId.hpp"
#include "Core/StringFormat.hpp"
#include "Core/ETPrimitives.hpp"
#include "Environment.hpp"

namespace {

class ET_TestInterface {
public:
    virtual ~ET_TestInterface() = default;
    virtual void ET_onEvent() {} 
    virtual void ET_OnParamEvent(int param) {}
    virtual int ET_OnReturnParamValue() { return -1; }
};

class TestETNode : public ETNode<ET_TestInterface> {
public:

    TestETNode() : eventRecieved(0) {}
    virtual ~TestETNode() {}

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
    node.connect(1);
    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node.eventRecieved, 1);

    node.disconnect();
    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node.eventRecieved, 1);
}

TEST_F(CoreTests, CheckBaseEventLogicToTwoListener) {
    TestETNode node1;
    node1.connect(1);

    TestETNode node2;
    node2.connect(1);
    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node1.eventRecieved, 1);
    ASSERT_EQ(node2.eventRecieved, 1);

    node1.disconnect();
    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node1.eventRecieved, 1);
    ASSERT_EQ(node2.eventRecieved, 2);

    node2.disconnect();

    ASSERT_EQ(node1.eventRecieved, 1);
    ASSERT_EQ(node2.eventRecieved, 2);
}

TEST_F(CoreTests, CheckAutoUnsubscribeAfterDelete) {
    std::unique_ptr<TestETNode> nodePtr(new TestETNode);
    nodePtr->connect(1);

    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);
    ASSERT_EQ(nodePtr->eventRecieved, 1);

    nodePtr.reset();

    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);
}

TEST_F(CoreTests, CheckDoubleConnect) {
    TestETNode node;
    node.connect(1);
    node.connect(1);
    ET_SendEvent(1, &ET_TestInterface::ET_onEvent);

    ASSERT_EQ(node.eventRecieved, 1);
}

TEST_F(CoreTests, CheclWithAruments) {
    TestETNode node;
    node.connect(1);
    ET_SendEvent(1, &ET_TestInterface::ET_OnParamEvent, 3);

    ASSERT_EQ(node.eventRecieved, 1);
}

TEST_F(CoreTests, CheckWithReturnValue) {
    TestETNode node;
    node.connect(1);

    int retVal = 0;
    ET_SendEventReturn(retVal, 1, &ET_TestInterface::ET_OnReturnParamValue);
    
    ASSERT_EQ(node.eventRecieved, retVal);
}