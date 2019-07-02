#include "ETSystemTests.hpp"

#include <Core/ETSystem.hpp>
#include <Core/ETPrimitives.hpp>

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

struct TestETInterface {
    virtual ~TestETInterface() = default;
    virtual TestObject ET_DoSomething(TestObject& inObj) = 0;
};

struct TestETNode : public ETNode<TestETInterface> {
    virtual ~TestETNode() = default;

    TestObject ET_DoSomething(TestObject& inObj) override {
        inObj.doSomething();
        TestObject outObj;
        outObj.doSomething();
        return outObj;
    }
};

} // namespace

TEST_F(ETSystemTests, CheckEnv) {
    std::unique_ptr<ETSystem> etSystem(new ETSystem);

    ASSERT_EQ(etSystem.get(), GetETSystem());

    etSystem.reset();

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
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
    
    TestETNode node;
    etSystem->connectNode(node, InvalidEntityId);

    ASSERT_FALSE(ET_IsExistNode<TestETInterface>(InvalidEntityId));
    ASSERT_FALSE(ET_IsExistNode<TestETInterface>());
}

TEST_F(ETSystemTests, CheckDobuleConnect) {
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
    
    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);
    etSystem->connectNode(node, entId);

    auto etNodes = ET_GetAll<TestETInterface>();
    ASSERT_EQ(etNodes.size(), 1u);
}

TEST_F(ETSystemTests, CheckDoubleDisconnect) {
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
    
    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);

    ASSERT_TRUE(ET_IsExistNode<TestETInterface>(entId));

    etSystem->disconnectNode(node);
    etSystem->disconnectNode(node);

    ASSERT_FALSE(ET_IsExistNode<TestETInterface>(entId));
}

TEST_F(ETSystemTests, CheckGetAll) {
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
    
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
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
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

TEST_F(ETSystemTests, CheckSendEventAfterDisconect) {
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
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
    std::unique_ptr<ETSystem> etSystem(new ETSystem);

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
    std::unique_ptr<ETSystem> etSystem(new ETSystem);
    auto entId = etSystem->createNewEntityId();
    TestETNode node;
    etSystem->connectNode(node, entId);

    TestObject inObj;
    TestObject outObj;
    etSystem->sendEventReturn(outObj, entId, &TestETInterface::ET_DoSomething, inObj);

    EXPECT_EQ(outObj.doSomethingCount, 1u);
    EXPECT_EQ(outObj.voidConstructCount, 1u);
    EXPECT_EQ(outObj.copyConstructCount, 0u);
    EXPECT_EQ(outObj.moveConstructCount, 0u);
    EXPECT_EQ(outObj.copyAssingCount, 0u);
    EXPECT_EQ(outObj.moveAssignCount, 1u);
}