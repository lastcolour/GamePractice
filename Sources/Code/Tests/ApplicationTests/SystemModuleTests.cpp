#include "SystemModuleTests.hpp"
#include "Core/SystemLogic.hpp"
#include "Core/SystemModule.hpp"
#include "Core/Utils.hpp"

#include <memory>
#include <tuple>
#include <iostream>

namespace {

class TestSystemLogic : public SystemLogic {
public:

    static bool INIT_RES;

public:

    TestSystemLogic() = default;
    virtual ~TestSystemLogic() = default;

    // SystemLogic
    bool init() override {
        return INIT_RES;
    }
    void deinit() override {}
};

bool TestSystemLogic::INIT_RES = true;

class TestSystemModule : public SystemModule {
public:

    TestSystemModule() : SystemModule("TestModule") {}
    virtual ~TestSystemModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override {
        return nullptr;
    }

    LogicsContainerPtrT getSystemLogics() const override {
        std::unique_ptr<SystemLogicContainerBase> container(new SystemLogicContainer<
            TestSystemLogic>());
        return container;
    }

    void registerEntityLogics(EntityLogicRegister& logicRegister) const {}
};

} // namespace

void SystemModuleTests::SetUp() {
    TestSystemLogic::INIT_RES = true;
}

void SystemModuleTests::TearDown() {
}

TEST_F(SystemModuleTests, TestSystemModuleInitSuccess) {
    TestSystemModule module;
    ASSERT_TRUE(module.init());
}

TEST_F(SystemModuleTests, TestSystemModuleInitFail) {
    TestSystemLogic::INIT_RES = false;
    TestSystemModule module;
    ASSERT_FALSE(module.init());
}