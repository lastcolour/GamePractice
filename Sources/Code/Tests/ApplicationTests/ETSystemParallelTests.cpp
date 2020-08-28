#include "ETSystemParallelTests.hpp"
#include "Core/ETPrimitives.hpp"

#include <thread>
#include <atomic>
#include <chrono>

namespace {

struct ETObject {
    virtual ~ETObject() = default;
    virtual void ET_update() = 0;
    virtual void ET_triggerUpdate() = 0;
};

class TestObject : public ETNode<ETObject> {
public:

    TestObject() : updated(0) {}
    virtual ~TestObject() {}

    void connect(ETSystem& etSystem) {
        entId = etSystem.createNewEntityId();
        etSystem.connectNode(*this, entId);
    }

    void ET_triggerUpdate() override {
        ET_SendEvent(entId, &ETObject::ET_update);
    }

    void ET_update() override {
        // ++updated;
    }

public:

    EntityId entId;
    int updated;
};

} // namespace

TEST_F(ETSystemParallelTests, CheckConnectDuringUpdate) {
    ETSystem etSystem;

    std::atomic<int> waitThtread(2);

    std::thread t1([&waitThtread](){
        while(waitThtread.load() != 0) {
            ET_SendEvent(&ETObject::ET_triggerUpdate);
        }
    });

    std::vector<std::unique_ptr<TestObject>> objects1;
    std::vector<std::unique_ptr<TestObject>> objects2;

    std::thread t2([&waitThtread, &objects1, &etSystem](){
        while(objects1.size() < 1000) {
            auto obj = objects1.emplace_back(new TestObject).get();
            obj->connect(etSystem);
            
        }
        waitThtread.fetch_sub(1);
    });

    std::thread t3([&waitThtread, &objects2, &etSystem](){
        while(objects2.size() < 1000) {
            auto obj = objects2.emplace_back(new TestObject).get();
            etSystem.connectNode(*obj, etSystem.createNewEntityId());
        }
        waitThtread.fetch_sub(1);
    });

    t1.join();
    t2.join();
    t3.join();

    auto all = etSystem.getAll<ETObject>();
    EXPECT_EQ(all.size(), 2000);
}

TEST_F(ETSystemParallelTests, CheckConnectDisconnectDuringUpdate) {
    ETSystem etSystem;

    const int MAX_OBJECTS = 2;

    std::atomic<int> waitThtread(2);

    std::thread t1([&waitThtread](){
        while(waitThtread.load() != 0) {
            ET_SendEvent(&ETObject::ET_triggerUpdate);
        }
    });

    std::vector<std::unique_ptr<TestObject>> objects1;
    for(int i = 0; i < MAX_OBJECTS; ++i) {
        auto obj = objects1.emplace_back(new TestObject).get();
        obj->connect(etSystem);
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto all = etSystem.getAll<ETObject>();
    EXPECT_EQ(all.size(), MAX_OBJECTS);

    std::vector<std::unique_ptr<TestObject>> objects2;

    std::thread t2([&waitThtread, &objects1, &etSystem](){
        while(!objects1.empty()) {
            objects1.pop_back();
        }
        waitThtread.fetch_sub(1);
    });

    std::thread t3([&waitThtread, &objects2, &etSystem, MAX_OBJECTS](){
        while(objects2.size() < MAX_OBJECTS) {
            auto obj = objects2.emplace_back(new TestObject).get();
            obj->connect(etSystem);
        }
        waitThtread.fetch_sub(1);
    });

    t1.join();
    t2.join();
    t3.join();

    all = etSystem.getAll<ETObject>();
    EXPECT_EQ(all.size(), MAX_OBJECTS);
}