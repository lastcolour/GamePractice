#ifndef __CONSOLE_APP_TESTS_HPP__
#define __CONSOLE_APP_TESTS_HPP__

#include <Entity/Entity.hpp>

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
        object->addCustomLogic(std::unique_ptr<LogicType>(new LogicType));
        return logic;
    }

private:

    std::vector<std::unique_ptr<Entity>> tempObject;
}; 

#endif /* __CONSOLE_APP_TESTS_HPP__ */

