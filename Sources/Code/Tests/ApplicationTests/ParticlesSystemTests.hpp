#ifndef __PARTICLES_SYSTEM_TESTS_HPP__
#define __PARTICLES_SYSTEM_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class ParticlesSystemTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckRender);
    }
};

#endif /* __PARTICLES_SYSTEM_TESTS_HPP__ */