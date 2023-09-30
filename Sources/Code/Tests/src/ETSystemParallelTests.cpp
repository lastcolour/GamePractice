#include "ETSystemParallelTests.hpp"

#include <thread>
#include <atomic>
#include <chrono>

using namespace ET;

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
        etSystem->connectNode(ET::GetETId<ETObject>(), static_cast<ETNode<ETObject>*>(this), entId);
        etSystem->connectNode(ET::GetETId<ETAltObject>(), static_cast<ETNode<ETAltObject>*>(this), entId);
    }

    void disconnect() {
        etSystem->disconnectNode(ET::GetETId<ETObject>(), static_cast<ETNode<ETObject>*>(this));
    }

    void ET_altTriggerConnect() override {
        etSystem->connectNode(ET::GetETId<ETObject>(), static_cast<ETNode<ETObject>*>(this), entId);
    }

    void ET_triggerDisconnect() override {
        etSystem->disconnectNode(ET::GetETId<ETObject>(), static_cast<ETNode<ETObject>*>(this));
    }

    void ET_triggerUpdate() override {
        auto it = etSystem->createIterator(ET::GetETId<ETObject>(), entId);
        while(it) {
            auto obj = static_cast<ETNode<ETObject>*>(*it);
            obj->ET_update();
            ++it;
        }
        etSystem->destroyIterator(it);
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
    Core::GlobalEnvironment env;
    ETSystem& etSystem = *env.GetETSystem();

    const int MAX_OBJECTS_COUNT = 200;

    std::atomic<int> waitThtread(2);

    std::thread t1([&waitThtread, &etSystem, MAX_OBJECTS_COUNT](){
        while(waitThtread.load() != 0) {
            auto it = etSystem.createIterator(ET::GetETId<ETObject>());
            while(it) {
                auto obj = static_cast<ETNode<ETObject>*>(*it);
                obj->ET_triggerUpdate();
                ++it;
            }
            etSystem.destroyIterator(it);
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
    Core::GlobalEnvironment env;
    ETSystem& etSystem = *env.GetETSystem();

    const int MAX_OBJECTS = 200;

    std::atomic<int> waitThtread(2);

    std::thread t1([&waitThtread, &etSystem](){
        while(waitThtread.load() != 0) {
            auto it = etSystem.createIterator(ET::GetETId<ETObject>());
            while(it) {
                auto obj = static_cast<ETNode<ETObject>*>(*it);
                obj->ET_triggerUpdate();
                ++it;
            }
            etSystem.destroyIterator(it);
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
    Core::GlobalEnvironment env;
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
            auto it = etSystem.createIterator(ET::GetETId<ETObject>());
            while(it) {
                auto obj = static_cast<ETNode<ETObject>*>(*it);
                obj->ET_triggerDisconnect();
                ++it;
            }
            etSystem.destroyIterator(it);
        }
    });

    std::thread t2([&runFlag, &objects, &etSystem, CONNECT_LOOPS](){
        for(int i = 0; i < CONNECT_LOOPS; ++i) {
            for(auto& etObj : objects) {
                auto it = etSystem.createIterator(ET::GetETId<ETAltObject>(), etObj->entId);
                while(it) {
                    auto obj = static_cast<ETNode<ETAltObject>*>(*it);
                    obj->ET_altTriggerConnect();
                    ++it;
                }
                etSystem.destroyIterator(it);
            }
        }
        runFlag.store(false);
    });

    t1.join();
    t2.join();

    {
        auto it = etSystem.createIterator(ET::GetETId<ETAltObject>());
        while(it) {
            auto obj = static_cast<ETNode<ETAltObject>*>(*it);
            obj->ET_altTriggerConnect();
            ++it;
        }
        etSystem.destroyIterator(it);
    }

    auto etObjects = etSystem.getAll<ETObject>();
    EXPECT_EQ(etObjects.size(), MAX_OBJECTS);

    objects.clear();
}