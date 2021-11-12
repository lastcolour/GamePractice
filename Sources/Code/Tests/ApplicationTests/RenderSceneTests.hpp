#ifndef __RENDER_SCENE_TESTS_HPP__
#define __RENDER_SCENE_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class RenderSceneTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckRender);
    }
};

#endif /* __RENDER_SCENE_TESTS_HPP__ */