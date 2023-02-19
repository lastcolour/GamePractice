#ifndef __RENDER_PARTICLES_TESTS_HPP__
#define __RENDER_PARTICLES_TESTS_HPP__

#include "RenderTests/RenderTests.hpp"

class RenderParticlesTests : public RenderTests {
protected:

    static void SetUpTestCase() {
        RenderTests::SetUpTestCase();
    }

    static void TearDownTestCase() {
        RenderTests::TearDownTestCase();
    }

    static void TickParticleSystems(float dt);
};

#endif /* __RENDER_PARTICLES_TESTS_HPP__ */