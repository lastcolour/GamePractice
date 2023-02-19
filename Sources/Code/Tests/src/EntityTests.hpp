#ifndef __ENTITY_TESTS_HPP__
#define __ENTITY_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class EntityTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckRender);
    }
};

#endif /* __ENTITY_TESTS_HPP__ */