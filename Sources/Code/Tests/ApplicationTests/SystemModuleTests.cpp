#include "SystemModuleTests.hpp"
#include "Core/SystemLogic.hpp"
#include "Core/SystemModule.hpp"
#include "Core/Utils.hpp"
#include "Core/JSONNode.hpp"
#include "Core/ETApplication.hpp"
#include "Core/GlobalData.hpp"

#include <tuple>
#include <iostream>

namespace {

class TestSystemLogic : public SystemLogic {
public:

    static bool INIT_RES;
    static bool DEINIT_CALLED;

public:

    TestSystemLogic() = default;
    virtual ~TestSystemLogic() = default;

    // SystemLogic
    bool init() override {
        return INIT_RES;
    }
    void deinit() override {
        DEINIT_CALLED = true;
    }
};

class TestSystemConfigs {
public:

    static void Reflect(ReflectContext& ctx) {
        if(auto classInfo = ctx.classInfo<TestSystemConfigs>("TestSystemConfigs")) {
            classInfo->addField("configVal", &TestSystemConfigs::configVal);
        }
    }

public:

    int configVal;
};

bool TestSystemLogic::INIT_RES = true;
bool TestSystemLogic::DEINIT_CALLED = false;

class TestSystemModule : public SystemModule {
public:

    TestSystemModule() : SystemModule("TestModule") {}
    virtual ~TestSystemModule() = default;

protected:

    void createSystemConfigs() const override {
        CreateGlobal<TestSystemConfigs>();
    }

    LogicsContainerPtrT createSystemLogics() const override {
        std::unique_ptr<SystemLogicContainerBase> container(new SystemLogicContainer<
            TestSystemLogic>());
        return container;
    }

    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const {}

protected:

    /*
    JSONNode loadModuleConfigs() override {
        return JSONNode::ParseString("{ \"TestSystemConfigs\": { \"configVal\" : 1 } }");
    }
    */
};

} // namespace

void SystemModuleTests::SetUp() {
    TestSystemLogic::INIT_RES = true;
    TestSystemLogic::DEINIT_CALLED = false;
}

void SystemModuleTests::TearDown() {
}

TEST_F(SystemModuleTests, TestSystemModuleInitSuccess) {
    TestSystemModule module;
    ASSERT_TRUE(module.init());

    auto config = GetGlobal<TestSystemConfigs>();
    ASSERT_TRUE(config);
    ASSERT_EQ(config->configVal, 1);
}

TEST_F(SystemModuleTests, TestSystemModuleInitFail) {
    TestSystemLogic::INIT_RES = false;
    std::unique_ptr<TestSystemModule> module(new TestSystemModule);
    ASSERT_FALSE(module->init());
    module.reset();
    ASSERT_TRUE(TestSystemLogic::DEINIT_CALLED);
}

TEST_F(SystemModuleTests, CheckDeinitNotCalled) {
    std::unique_ptr<TestSystemModule> module(new TestSystemModule);
    module.reset();
    ASSERT_FALSE(TestSystemLogic::DEINIT_CALLED);
}