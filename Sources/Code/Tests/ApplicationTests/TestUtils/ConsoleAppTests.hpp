#ifndef __CONSOLE_APP_TESTS_HPP__
#define __CONSOLE_APP_TESTS_HPP__

#include <Game/GameObject.hpp>

#include <gtest/gtest.h>
#include <memory>

class Application;

class ConsoleAppTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

    void TearDown() override;

protected:

    static std::unique_ptr<GameObject> createVoidObject();

    template<typename LogicType>
    LogicType* createObjectAndLogic() {
        LogicType* logic = new LogicType;
        std::unique_ptr<GameLogic> logicPtr(logic);
        auto object = createVoidObject();
        addLogicToOject(object, logicPtr);
        tempObject.push_back(std::move(object));
        return logic;
    }

protected:

    static std::unique_ptr<Application> APP;

private:

    void addLogicToOject(std::unique_ptr<GameObject>& obj, std::unique_ptr<GameLogic>& logic);

private:

    std::vector<std::unique_ptr<GameObject>> tempObject;
}; 

#endif /* __CONSOLE_APP_TESTS_HPP__ */

