#include "GLFWSurfaceTests.hpp"
#include "Platforms/Desktop/GLFWSurface.hpp"

TEST_F(GLFWSurfaceTest, CheckNormalFlow) {
    GLFWSurface surface;
    bool res = surface.init();
    ASSERT_TRUE(res);

    auto size = surface.ET_getSize();

    ASSERT_GT(size.x, 0);
    ASSERT_GT(size.y, 0);

    auto glCtxType = surface.ET_getGLContextType();

    ASSERT_NE(glCtxType, GLContextType::None);

    auto isVisible = surface.ET_isVisible();
    ASSERT_FALSE(isVisible);

    surface.ET_show();
    isVisible = surface.ET_isVisible();
    EXPECT_TRUE(isVisible);

    surface.ET_onTick(0.f);

    surface.ET_hide();
    isVisible = surface.ET_isVisible();
    ASSERT_FALSE(isVisible);

    EXPECT_TRUE(surface.ET_isValid());

    surface.ET_close();

    EXPECT_FALSE(surface.ET_isValid());
}
