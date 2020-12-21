#include "ETSystemTests.hpp"
#include "Core/ETSystem.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/GlobalEnvironment.hpp"

namespace {

class TestObject {
public:

    TestObject() {
        voidConstructCount = 1u;
        copyConstructCount = 0u;
        moveConstructCount = 0u;
        copyAssingCount = 0u;
        moveAssignCount = 0u;
        doSomethingCount = 0u;
    }
    TestObject(const TestObject& obj) {
        copyState(obj);
        ++copyConstructCount;
    }
    TestObject(TestObject&& obj) {
        copyState(obj);
        ++moveConstructCount;
    }
    TestObject& operator=(const TestObject& obj) {
        copyState(obj);
        ++copyAssingCount;
        return *this;
    }
    TestObject& operator=(TestObject&& obj) {
        copyState(obj);
        ++moveAssignCount;
        return *this;
    }
    void doSomething() {
        ++doSomethingCount;
    }

private:

    void copyState(const TestObject& obj) {
        voidConstructCount = obj.voidConstructCount;
        copyConstructCount = obj.copyConstructCount;
        moveConstructCount = obj.moveConstructCount;
        copyAssingCount = obj.copyAssingCount;
        moveAssignCount = obj.moveAssignCount;
        doSomethingCount = obj.doSomethingCount;
    }

public:

    unsigned int doSomethingCount;
    unsigned int voidConstructCount;
    unsigned int copyConstructCount;
    unsigned int moveConstructCount;
    unsigned int copyAssingCount;
    unsigned int moveAssignCount;
};

struct TestETNode;

struct TestETInterface {
    virtual ~TestETInterface() = default;
    virtual TestObject ET_DoSomething(TestObject& inObj) = 0;
    virtual void ET_CreateObject(size_t count, std::vector<std::unique_ptr<TestETNode>>& objects, ETSystem& etSystem) = 0;
    virtual void ET_DisconnectObject(TestETNode& etNode, ETSystem& etSystem) = 0;
    virtual void ET_IncreaseCounter(int num) = 0;
    virtual void ET_acquireObject(std::unique_ptr<TestObject>& obj) = 0;
};

struct TestETNode : public ETNode<TestETInterface> {

    TestETNode() : counter(0) {}
    virtual ~TestETNode() = default;

    TestObject ET_DoSomething(TestObject& inObj) override {
        inObj.doSomething();
        TestObject outObj;
        outObj.doSomething();
        return outObj;
    }

    void ET_CreateObject(size_t count, std::vector<std::unique_ptr<TestETNode>>& objects, ETSystem& etSystem) override {
        for(size_t i = 0; i < count; ++i) {
            auto entId = etSystem.createNewEntityId();
            std::unique_ptr<TestETNode> node(new TestETNode);
            etSystem.connectNode(*node, entId);
            objects.push_back(std::move(node));
        }
    }

    void ET_DisconnectObject(TestETNode& etNode, ETSystem& etSystem) override {
        etSystem.disconnectNode(etNode);
    }

    void ET_IncreaseCounter(int num) override {
        counter += num;
    }

    void ET_acquireObject(std::unique_ptr<TestObject>& obj) override {
        object = std::move(obj);
    }

public:

    int counter;
    std::unique_ptr<TestObject> object;
};

} // namespace

TEST_F(ETSystemTests, CheckEnv) {
    std::unique_ptr<GlobalEnvironment> env(new GlobalEnvironment);
    auto etSystem = env->GetETSystem();

    ASSERT_EQ(etSystem, GetETSystem());

    env.reset();

    ASSERT_EQ(GetETSystem(), nullptr);
}

TEST_F(ETSystemTests, CheckCretaeEntityId) {
    std::unique_ptr<ETSystem> etSystem(new ETSystem);

    auto entId_1 = etSystem->createNewEntityId();
    ASSERT_NE(entId_1, InvalidEntityId);

    auto entId_2 = etSystem->createNewEntityId();
    ASSERT_NE(entId_2, InvalidEntityId);

    ASSERT_NE(entId_1, entId_2);
}

TEST_F(ETSystemTests, CheckConnectToInvalidAddressId) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    TestETNode node;
    etSystem->connectNode(node, InvalidEntityId);

    ASSERT_FALSE(ET_IsExistNode<TestETInterface>(InvalidEntityId));
    ASSERT_FALSE(ET_IsExistNode<TestETInterface>());
}

TEST_F(ETSystemTests, CheckDobuleConnect) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);
    etSystem->connectNode(node, entId);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 1u);
}

TEST_F(ETSystemTests, CheckDoubleDisconnect) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);

    ASSERT_TRUE(ET_IsExistNode<TestETInterface>(entId));

    etSystem->disconnectNode(node);
    etSystem->disconnectNode(node);

    ASSERT_FALSE(ET_IsExistNode<TestETInterface>(entId));
}

TEST_F(ETSystemTests, CheckGetAll) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(node_2, entId_2);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 2u);

    ASSERT_EQ(etNodes[0], entId_1);
    ASSERT_EQ(etNodes[1], entId_2);
}

TEST_F(ETSystemTests, CheckSendEvent) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);

    TestObject inObj;
    etSystem->sendEvent(entId, &TestETInterface::ET_DoSomething, inObj);

    EXPECT_EQ(inObj.doSomethingCount, 1u);
    EXPECT_EQ(inObj.voidConstructCount, 1u);
    EXPECT_EQ(inObj.copyConstructCount, 0u);
    EXPECT_EQ(inObj.moveConstructCount, 0u);
    EXPECT_EQ(inObj.copyAssingCount, 0u);
    EXPECT_EQ(inObj.moveAssignCount, 0u);
}

TEST_F(ETSystemTests, CheckSendEventAfterDisconnect) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);
    etSystem->disconnectNode(node);

    TestObject inObj;
    etSystem->sendEvent(entId, &TestETInterface::ET_DoSomething, inObj);

    EXPECT_EQ(inObj.doSomethingCount, 0u);
    EXPECT_EQ(inObj.voidConstructCount, 1u);
    EXPECT_EQ(inObj.copyConstructCount, 0u);
    EXPECT_EQ(inObj.moveConstructCount, 0u);
    EXPECT_EQ(inObj.copyAssingCount, 0u);
    EXPECT_EQ(inObj.moveAssignCount, 0u);
}

TEST_F(ETSystemTests, CheckSendEventAll) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(node_2, entId_2);

    TestObject inObj;
    etSystem->sendEvent(&TestETInterface::ET_DoSomething, inObj);

    EXPECT_EQ(inObj.doSomethingCount, 2u);
    EXPECT_EQ(inObj.voidConstructCount, 1u);
    EXPECT_EQ(inObj.copyConstructCount, 0u);
    EXPECT_EQ(inObj.moveConstructCount, 0u);
    EXPECT_EQ(inObj.copyAssingCount, 0u);
    EXPECT_EQ(inObj.moveAssignCount, 0u);
}

TEST_F(ETSystemTests, CheckSendEventReturn) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);

    TestObject inObj;
    TestObject outObj;
    etSystem->sendEventReturn(outObj, entId, &TestETInterface::ET_DoSomething, inObj);

    EXPECT_EQ(outObj.doSomethingCount, 1u);
    EXPECT_EQ(outObj.voidConstructCount, 1u);
    EXPECT_EQ(outObj.copyConstructCount, 0u);
    // Test Result between MSVC & GCC differs in this place
    // EXPECT_EQ(outObj.moveConstructCount, 0u);
    EXPECT_EQ(outObj.copyAssingCount, 0u);
    EXPECT_EQ(outObj.moveAssignCount, 1u);
}

TEST_F(ETSystemTests, CheckRecursiveETNodeCreate) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();

    TestETNode node;
    etSystem->connectNode(node, entId);

    std::vector<std::unique_ptr<TestETNode>> testNodes;
    etSystem->sendEvent(&TestETInterface::ET_CreateObject, 10, testNodes, *etSystem);

    ASSERT_EQ(testNodes.size(), 10u);

    etSystem->sendEvent(&TestETInterface::ET_CreateObject, 10, testNodes, *etSystem);

    ASSERT_EQ(testNodes.size(), 120u);
}

TEST_F(ETSystemTests, CheckMultipleETNodesOnSameAdrress) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(node_1, entId);

    TestETNode node_2;
    etSystem->connectNode(node_2, entId);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 2u);

    TestObject inObj;
    etSystem->sendEvent(entId, &TestETInterface::ET_DoSomething, inObj);

    ASSERT_EQ(inObj.doSomethingCount, 2);
}

TEST_F(ETSystemTests, CheckActiveRounteDisconnect) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(node_2, entId_2);

    etSystem->sendEvent(entId_1, &TestETInterface::ET_DisconnectObject, node_2, *etSystem);

    auto activeConn = etSystem->getAll<TestETInterface>();
    ASSERT_EQ(activeConn.size(), 1u);
}

TEST_F(ETSystemTests, CheckQueueEvents) {
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(node_2, entId_2);

    etSystem->queueEvent(entId_1, &TestETInterface::ET_IncreaseCounter, 1);

    EXPECT_EQ(node_1.counter, 0);

    ET_PollAllEvents<TestETInterface>();

    EXPECT_EQ(node_1.counter, 1);

    etSystem->queueEvent(&TestETInterface::ET_IncreaseCounter, 1);

    EXPECT_EQ(node_2.counter, 0);

    ET_PollAllEvents<TestETInterface>();

    EXPECT_EQ(node_1.counter, 2);
    EXPECT_EQ(node_2.counter, 1);
}

/*
TEST_F(ETSystemTests, CheckQueueEventWithNonCopybaleObject) {
    // require perfect lambda capture with support of transfering ownership of an object
    GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto addressId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, addressId);

    EXPECT_FALSE(node.object.get());

    std::unique_ptr<TestObject> testObject(new TestObject);
    etSystem->queueEvent(&TestETInterface::ET_acquireObject, std::move(testObject));

    ET_PollAllEvents<TestETInterface>();

    EXPECT_TRUE(node.object.get());
}
*/