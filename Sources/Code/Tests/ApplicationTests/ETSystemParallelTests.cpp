#include "ETSystemParallelTests.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/GlobalEnvironment.hpp"

#include <thread>
#include <atomic>
#include <chrono>

namespace {

struct ETObject {
    virtual ~ETObject() = default;
    virtual void ET_update() = 0;
    virtual void ET_triggerUpdate() = 0;
    virtual void ET_triggerDisconnect() = 0;
};

struct ETAltObject {
    virtual ~ETAltObject() = default;
    virtual void ET_altTriggerConnect() = 0;
};

class TestObject : public ETNode<ETObject>,
    public ETNode<ETAltObject> {
public:

    TestObject(ETSystem& et) : etSystem(&et), updated(0) {
        entId = etSystem->createNewEntityId();
    }
    virtual ~TestObject() {
    }

    void connect() {
        etSystem->connectNode(static_cast<ETNode<ETObject>&>(*this), entId);
        etSystem->connectNode(static_cast<ETNode<ETAltObject>&>(*this), entId);
    }

    void disconnect() {
        ETNode<ETObject>::disconnect();
    }

    void ET_altTriggerConnect() override {
        etSystem->connectNode(static_cast<ETNode<ETObject>&>(*this), entId);
    }

    void ET_triggerDisconnect() override {
        ETNode<ETObject>::disconnect();
    }

    void ET_triggerUpdate() override {
        etSystem->sendEvent(entId, &ETObject::ET_update);

    }

    void ET_update() override {
        ++updated;
    }

public:

    ETSystem* etSystem;
    EntityId entId;
    int updated;
};

} // namespace

TEST_F(ETSystemParallelTests, CheckConnectDuringUpdate) {
    GlobalEnvironment env;
    ETSystem& etSystem = *env.GetETSystem();

    const int MAX_OBJECTS_COUNT = 200;

    std::atomic<int> waitThtread(2);

    std::thread t1([&waitThtread, &etSystem, MAX_OBJECTS_COUNT](){
        while(waitThtread.load() != 0) {
            etSystem.sendEvent(&ETObject::ET_triggerUpdate);
        }
    });

    std::vector<std::unique_ptr<TestObject>> objects1;
    std::vector<std::unique_ptr<TestObject>> objects2;

    std::thread t2([&waitThtread, &objects1, &etSystem, MAX_OBJECTS_COUNT](){
        while(objects1.size() < MAX_OBJECTS_COUNT) {
            auto obj = objects1.emplace_back(new TestObject(etSystem)).get();
            obj->connect();
        }
        waitThtread.fetch_sub(1);
    });

    std::thread t3([&waitThtread, &objects2, &etSystem, MAX_OBJECTS_COUNT](){
        while(objects2.size() < MAX_OBJECTS_COUNT) {
            auto obj = objects2.emplace_back(new TestObject(etSystem)).get();
            obj->connect();
        }
        waitThtread.fetch_sub(1);
    });

    t1.join();
    t2.join();
    t3.join();

    auto all = etSystem.getAll<ETObject>();
    if(all.size() < MAX_OBJECTS_COUNT * 2) {
        ASSERT_EQ(all.size(), MAX_OBJECTS_COUNT * 2);
    }

    objects1.clear();
    objects2.clear();
}

TEST_F(ETSystemParallelTests, CheckConnectDisconnectDuringUpdate) {
    GlobalEnvironment env;
    ETSystem& etSystem = *env.GetETSystem();

    const int MAX_OBJECTS = 200;

    std::atomic<int> waitThtread(2);

    std::thread t1([&waitThtread, &etSystem](){
        while(waitThtread.load() != 0) {
            etSystem.sendEvent(&ETObject::ET_triggerUpdate);
        }
    });

    std::vector<std::unique_ptr<TestObject>> objects1;
    for(int i = 0; i < MAX_OBJECTS; ++i) {
        auto obj = objects1.emplace_back(new TestObject(etSystem)).get();
        obj->connect();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto all = etSystem.getAll<ETObject>();
    EXPECT_EQ(all.size(), MAX_OBJECTS);

    std::vector<std::unique_ptr<TestObject>> objects2;

    std::thread t2([&waitThtread, &objects1, &etSystem](){
        while(!objects1.empty()) {
            auto& obj = objects1.back();
            obj->disconnect();
            objects1.pop_back();
        }
        waitThtread.fetch_sub(1);
    });

    std::thread t3([&waitThtread, &objects2, &etSystem, MAX_OBJECTS](){
        while(objects2.size() < MAX_OBJECTS) {
            auto obj = objects2.emplace_back(new TestObject(etSystem)).get();
            obj->connect();
        }
        waitThtread.fetch_sub(1);
    });

    t1.join();
    t2.join();
    t3.join();

    all = etSystem.getAll<ETObject>();
    EXPECT_EQ(all.size(), MAX_OBJECTS);

    objects1.clear();
    objects2.clear();
}

TEST_F(ETSystemParallelTests, CheckIndirectConnectDisconnect) {
    GlobalEnvironment env;
    ETSystem& etSystem = *env.GetETSystem();

    const int MAX_OBJECTS = 200;
    const int CONNECT_LOOPS = 200;

    std::atomic<bool> runFlag;
    runFlag.store(true);

    std::vector<std::unique_ptr<TestObject>> objects;
    for(int i = 0; i < MAX_OBJECTS; ++i) {
        auto& obj = objects.emplace_back(new TestObject(etSystem));
        obj->connect();
    }

    std::thread t1([&runFlag, &etSystem](){
        while(runFlag.load()){
            etSystem.sendEvent(&ETObject::ET_triggerDisconnect);
        }
    });

    std::thread t2([&runFlag, &objects, &etSystem, CONNECT_LOOPS](){
        for(int i = 0; i < CONNECT_LOOPS; ++i) {
            for(auto& obj : objects) {
                etSystem.sendEvent(obj->entId, &ETAltObject::ET_altTriggerConnect);
            }
        }
        runFlag.store(false);
    });

    t1.join();
    t2.join();

    etSystem.sendEvent(&ETAltObject::ET_altTriggerConnect);

    auto etObjects = etSystem.getAll<ETObject>();
    EXPECT_EQ(etObjects.size(), MAX_OBJECTS);

    objects.clear();
}