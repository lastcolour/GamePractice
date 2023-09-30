#ifndef __UI_ANIMATION_SEQUENCE_TESTS_HPP__
#define __UI_ANIMATION_SEQUENCE_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class UIAnimationSequenceTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckUI);
    }
};

#endif /* __UI_ANIMATION_SEQUENCE_TESTS_HPP__ */