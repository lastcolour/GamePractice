#ifndef __UI_LABEL_TESTS_HPP__
#define __UI_LABEL_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class UILabel;

class UILabelTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckRender);
    }

protected:

    UILabel* createUILabel();
};

#endif /* __UI_LABEL_TESTS_HPP__ */