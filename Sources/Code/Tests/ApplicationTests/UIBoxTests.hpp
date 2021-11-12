#ifndef __UI_BOX_TESTS_HPP__
#define __UI_BOX_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class UIBox;

class UIBoxTests : public EngineTests {
protected:

    static void SetUpTestCase();

protected:

    UIBox* createUIBox();
};

#endif /* __UI_BOX_TESTS_HPP__ */