#ifndef __CONSOLE_APP_TESTS_HPP__
#define __CONSOLE_APP_TESTS_HPP__

#include <Entity.hpp>

#include <gtest/gtest.h>
#include <memory>

class Application;

class ConsoleAppTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

    void TearDown() override;

protected:

    Entity* createVoidObject();

    template<typename LogicType>
    LogicType* createObjectAndLogic() {
        LogicType* logic = new LogicType;
        std::unique_ptr<EntityLogic> logicPtr(logic);
        auto object = createVoidObject();
        object->addCustomLogic(std::move(logicPtr));
        return logic;
    }

private:

    std::vector<EntityId> tempObject;
};

#endif /* __CONSOLE_APP_TESTS_HPP__ */

