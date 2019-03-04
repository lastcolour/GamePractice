#include "GLFWSurfaceTests.hpp"
#include "Platforms/Desktop/GLFWSurface.hpp"

TEST_F(GLFWSurfaceTest, TestInit) {
    GLFWSurface surface;
    bool res = surface.init();
    ASSERT_TRUE(res);

    auto size = surface.ET_getSize();

    ASSERT_GT(size.x, 0);
    ASSERT_GT(size.y, 0);
}

TEST_F(GLFWSurfaceTest, TestShouldRun) {
    GLFWSurface surface;
    ASSERT_TRUE(surface.init());
    bool res = surface.shouldRun();
    ASSERT_TRUE(res);
}

TEST_F(GLFWSurfaceTest, TestUpdate) {
    GLFWSurface surface;
    ASSERT_TRUE(surface.init());
    ASSERT_TRUE(surface.shouldRun());
    surface.update();
    surface.ET_terminate();
    ASSERT_FALSE(surface.shouldRun());
}
