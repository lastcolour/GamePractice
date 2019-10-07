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

    virtual void TearDown() override;

protected:

    static std::unique_ptr<Entity> createVoidObject();

    template<typename LogicType>
    LogicType* createObjectAndLogic() {
        LogicType* logic = new LogicType;
        std::unique_ptr<EntityLogic> logicPtr(logic);
        auto object = createVoidObject();
        addLogicToOject(object, logicPtr);
        tempObject.push_back(std::move(object));
        return logic;
    }

private:

    void addLogicToOject(std::unique_ptr<Entity>& obj, std::unique_ptr<EntityLogic>& logic);

private:

    std::vector<std::unique_ptr<Entity>> tempObject;
}; 

#endif /* __CONSOLE_APP_TESTS_HPP__ */

