#include "ETSystemTests.hpp"
#include "Core/GlobalEnvironment.hpp"

using namespace ET;

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
    virtual void ET_moveObject(TestObject&& obj) = 0;
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
            etSystem.connectNode(ET::GetETId<TestETInterface>(), node.get(), entId);
            objects.push_back(std::move(node));
        }
    }

    void ET_DisconnectObject(TestETNode& etNode, ETSystem& etSystem) override {
        etSystem.disconnectNode(ET::GetETId<TestETInterface>(), &etNode);
    }

    void ET_IncreaseCounter(int num) override {
        counter += num;
    }

    void ET_acquireObject(std::unique_ptr<TestObject>& obj) override {
        object = std::move(obj);
    }

    void ET_moveObject(TestObject&& obj) override {
        object.reset(new TestObject(std::move(obj)));
    }


public:

    int counter;
    std::unique_ptr<TestObject> object;
};

} // namespace

TEST_F(ETSystemTests, CheckEnv) {
    std::unique_ptr<Core::GlobalEnvironment> env(new Core::GlobalEnvironment);
    ASSERT_TRUE(env->GetETSystem());
}

TEST_F(ETSystemTests, CheckETIterator) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    std::vector<TestETNode> nodes{4};
    EntityId entIds[4];
    for(int i = 0; i < 4; ++i) {
        entIds[i] = etSystem->createNewEntityId();
        etSystem->connectNode(ET::GetETId<TestETInterface>(), &nodes[i], entIds[i]);
    }

    for(int i = 0; i < 4; ++i) {
        int count = 0;
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>(), entIds[i]);
        while(it) {
            ++count;
            ++it;
        }
        etSystem->destroyIterator(it);

        EXPECT_EQ(count, 1);
    }

    {
        int count = 0;
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>());
        while(it) {
            ++count;
            ++it;
        }
        etSystem->destroyIterator(it);

        EXPECT_EQ(count, 4);
    }

    etSystem->disconnectNode(ET::GetETId<TestETInterface>(), &nodes[1]);

    {
        int count = 0;
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>());
        while(it) {
            ++count;
            ++it;
        }
        etSystem->destroyIterator(it);

        EXPECT_EQ(count, 3);
    }

    etSystem->disconnectNode(ET::GetETId<TestETInterface>(), &nodes[3]);

    {
        int count = 0;
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>());
        while(it) {
            ++count;
            ++it;
        }
        etSystem->destroyIterator(it);

        EXPECT_EQ(count, 2);
    }

    nodes.clear();
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
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, InvalidEntityId);

    ASSERT_FALSE(ET_IsExistNode<TestETInterface>(InvalidEntityId));
    ASSERT_FALSE(ET_IsExistNode<TestETInterface>());
}

TEST_F(ETSystemTests, CheckDobuleConnect) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 1u);
}

TEST_F(ETSystemTests, CheckDoubleDisconnect) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);

    ASSERT_TRUE(ET_IsExistNode<TestETInterface>(entId));

    etSystem->disconnectNode(ET::GetETId<TestETInterface>(), &node);
    etSystem->disconnectNode(ET::GetETId<TestETInterface>(), &node);

    ASSERT_FALSE(ET_IsExistNode<TestETInterface>(entId));
}

TEST_F(ETSystemTests, CheckGetAll) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_2, entId_2);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 2u);

    auto it = std::find(etNodes.begin(), etNodes.end(), entId_1);
    ASSERT_TRUE(it != etNodes.end());

    it = std::find(etNodes.begin(), etNodes.end(), entId_2);
    ASSERT_TRUE(it != etNodes.end());
}

TEST_F(ETSystemTests, CheckSendEvent) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);

    TestObject inObj;

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>(), entId);
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_DoSomething(inObj);
            ++it;
        }
        etSystem->destroyIterator(it);
    }


    EXPECT_EQ(inObj.doSomethingCount, 1u);
    EXPECT_EQ(inObj.voidConstructCount, 1u);
    EXPECT_EQ(inObj.copyConstructCount, 0u);
    EXPECT_EQ(inObj.moveConstructCount, 0u);
    EXPECT_EQ(inObj.copyAssingCount, 0u);
    EXPECT_EQ(inObj.moveAssignCount, 0u);
}

TEST_F(ETSystemTests, CheckSendEventAfterDisconnect) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);
    etSystem->disconnectNode(ET::GetETId<TestETInterface>(), &node);

    TestObject inObj;

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>(), entId);
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_DoSomething(inObj);
            ++it;
        }
        etSystem->destroyIterator(it);
    }

    EXPECT_EQ(inObj.doSomethingCount, 0u);
    EXPECT_EQ(inObj.voidConstructCount, 1u);
    EXPECT_EQ(inObj.copyConstructCount, 0u);
    EXPECT_EQ(inObj.moveConstructCount, 0u);
    EXPECT_EQ(inObj.copyAssingCount, 0u);
    EXPECT_EQ(inObj.moveAssignCount, 0u);
}

TEST_F(ETSystemTests, CheckSendEventAll) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_2, entId_2);

    TestObject inObj;

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>());
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_DoSomething(inObj);
            ++it;
        }
        etSystem->destroyIterator(it);
    }

    EXPECT_EQ(inObj.doSomethingCount, 2u);
    EXPECT_EQ(inObj.voidConstructCount, 1u);
    EXPECT_EQ(inObj.copyConstructCount, 0u);
    EXPECT_EQ(inObj.moveConstructCount, 0u);
    EXPECT_EQ(inObj.copyAssingCount, 0u);
    EXPECT_EQ(inObj.moveAssignCount, 0u);
}

TEST_F(ETSystemTests, CheckSendEventReturn) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);

    TestObject inObj;
    TestObject outObj;

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>(), entId);
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            outObj = obj->ET_DoSomething(inObj);
            break;
        }
        etSystem->destroyIterator(it);
    }

    EXPECT_EQ(outObj.doSomethingCount, 1u);
    EXPECT_EQ(outObj.voidConstructCount, 1u);
    EXPECT_EQ(outObj.copyConstructCount, 0u);
    // Test Result between MSVC & GCC differs in this place
    // EXPECT_EQ(outObj.moveConstructCount, 0u);
    EXPECT_EQ(outObj.copyAssingCount, 0u);
    EXPECT_EQ(outObj.moveAssignCount, 1u);
}

TEST_F(ETSystemTests, CheckRecursiveETNodeCreate) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();

    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, entId);

    std::vector<std::unique_ptr<TestETNode>> testNodes;

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>());
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_CreateObject(10, testNodes, *etSystem);
            ++it;
        }
        etSystem->destroyIterator(it);
    }

    ASSERT_EQ(testNodes.size(), 10u);

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>());
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_CreateObject(10, testNodes, *etSystem);
            ++it;
        }
        etSystem->destroyIterator(it);
    }

    ASSERT_EQ(testNodes.size(), 120u);
}

TEST_F(ETSystemTests, CheckMultipleETNodesOnSameAdrress) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_1, entId);

    TestETNode node_2;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_2, entId);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 2u);

    TestObject inObj;

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>(), entId);
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_DoSomething(inObj);
            ++it;
        }
        etSystem->destroyIterator(it);
    }

    ASSERT_EQ(inObj.doSomethingCount, 2u);
}

TEST_F(ETSystemTests, CheckActiveRounteDisconnect) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_2, entId_2);

    {
        auto it = etSystem->createIterator(ET::GetETId<TestETInterface>(), entId_1);
        while(it) {
            auto obj = static_cast<ETNode<TestETInterface>*>(*it);
            obj->ET_DisconnectObject(node_2, *etSystem);
            ++it;
        }
        etSystem->destroyIterator(it);
    }

    auto activeConn = etSystem->getAll<TestETInterface>();
    ASSERT_EQ(activeConn.size(), 1u);
}

TEST_F(ETSystemTests, CheckQueueEvents) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto entId_1 = etSystem->createNewEntityId();
    TestETNode node_1;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_1, entId_1);

    auto entId_2 = etSystem->createNewEntityId();
    TestETNode node_2;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node_2, entId_2);

    {
        using ObjectT = ETNode<TestETInterface>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(&TestETInterface::ET_IncreaseCounter), int>;

        auto event = etSystem->createDefferedEvent<CallT>(entId_1, &TestETInterface::ET_IncreaseCounter, 1);
        etSystem->queueEvent(ET::GetETId<TestETInterface>(), event);
    }

    EXPECT_EQ(node_1.counter, 0);

    ET_PollAllEvents<TestETInterface>();

    EXPECT_EQ(node_1.counter, 1);

    {
        using ObjectT = ETNode<TestETInterface>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(&TestETInterface::ET_IncreaseCounter), int>;

        auto event = etSystem->createDefferedEvent<CallT>(InvalidEntityId, &TestETInterface::ET_IncreaseCounter, 1);
        etSystem->queueEvent(ET::GetETId<TestETInterface>(), event);
    }

    EXPECT_EQ(node_2.counter, 0);

    ET_PollAllEvents<TestETInterface>();

    EXPECT_EQ(node_1.counter, 2);
    EXPECT_EQ(node_2.counter, 1);
}

TEST_F(ETSystemTests, CheckQueueEventWithNonCopybaleObject) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto addressId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(ET::GetETId<TestETInterface>(), &node, addressId);

    EXPECT_FALSE(node.object.get());

    std::unique_ptr<TestObject> testObject(new TestObject);

    {
        using ObjectT = ETNode<TestETInterface>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(&TestETInterface::ET_acquireObject), std::unique_ptr<TestObject>>;

        auto event = etSystem->createDefferedEvent<CallT>(InvalidEntityId, &TestETInterface::ET_acquireObject, std::move(testObject));
        etSystem->queueEvent(ET::GetETId<TestETInterface>(), event);
    }

    ET_PollAllEvents<TestETInterface>();

    EXPECT_TRUE(node.object.get());
    EXPECT_FALSE(testObject.get());
}

/*
Require Perfcet Lambda Capture from C++20
TEST_F(ETSystemTests, CheckQueueEventMoveObject) {
    Core::GlobalEnvironment env;
    auto etSystem = env.GetETSystem();

    auto addressId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, addressId);

    EXPECT_FALSE(node.object.get());

    TestObject testObject;
    etSystem->queueEvent(&TestETInterface::ET_moveObject, std::move(testObject));

    ET_PollAllEvents<TestETInterface>();

    EXPECT_TRUE(node.object.get());

    EXPECT_EQ(node.object->voidConstructCount, 1u);
    EXPECT_EQ(node.object->copyConstructCount, 1u);
    EXPECT_EQ(node.object->moveConstructCount, 0u);
    EXPECT_EQ(node.object->copyAssingCount, 0u);
    EXPECT_EQ(node.object->moveAssignCount, 1u);
}
*/