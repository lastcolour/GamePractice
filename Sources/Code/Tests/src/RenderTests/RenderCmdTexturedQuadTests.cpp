#include "RenderTests/RenderCmdTexturedQuadTests.hpp"
#include "Render/src/Logics/RenderLinearGradientRect.hpp"
#include "Render/src/Logics/RenderImageLogic.hpp"

namespace {

const char* TEST_IMAGE = "Images/Base/block_1x1.png";

} // namespace

TEST_F(RenderCmdTexturedQuadTests, CheckTexture) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderImageLogic>();
    auto tintColor = ColorB(0, 255, 0, 255);

    {
        TextureInfo texInfo;
        texInfo.filename = TEST_IMAGE;
        texInfo.tintColor = tintColor;
        logic->ET_setTextureInfo(texInfo);
    }

    const Vec2 quadSize = Vec2(255.f);
    logic->ET_setSize(quadSize);
    logic->ET_show();
 
    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    syncWithGameAndDrawToImageBuffer();

    AABB2Di box;
    box.setCenterAndSize(getRenderResolution() / 2, Vec2i(255));
    EXPECT_EQ(0, countNotEqualPixels(box, tintColor));
}

TEST_F(RenderCmdTexturedQuadTests, CheckInvalidTexture) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderImageLogic>();

    const Vec2 quadSize = Vec2(255.f);
    logic->ET_setSize(quadSize);
    logic->ET_show();

    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    syncWithGameAndDrawToImageBuffer();

    AABB2Di box;
    box.setCenterAndSize(getRenderResolution() / 2, Vec2i(255));
    EXPECT_EQ(0, countNotEqualPixels(box, ColorB(0, 0, 0)));
}

TEST_F(RenderCmdTexturedQuadTests, CheckSizeUpdate) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderImageLogic>();
    auto tintColor = ColorB(0, 255, 0, 255);

    {
        TextureInfo texInfo;
        texInfo.filename = TEST_IMAGE;
        texInfo.tintColor = tintColor;
        logic->ET_setTextureInfo(texInfo);
    }

    Vec2 quadSize = Vec2(256.f);
    logic->ET_setSize(quadSize);
    logic->ET_show();
 
    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    syncWithGameAndDrawToImageBuffer();

    {
        AABB2Di box;
        box.setCenterAndSize(getRenderResolution() / 2, Vec2i(256));
        EXPECT_EQ(0, countNotEqualPixels(box, tintColor));
    }

    quadSize = Vec2(512.f);
    logic->ET_setSize(quadSize);

    syncWithGameAndDrawToImageBuffer();

    {
        AABB2Di box;
        box.setCenterAndSize(getRenderResolution() / 2, Vec2i(512));
        EXPECT_EQ(0, countNotEqualPixels(box, tintColor));
    }
}

TEST_F(RenderCmdTexturedQuadTests, CheckScaleUpdate) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderImageLogic>();
    auto tintColor = ColorB(0, 255, 0, 255);

    {
        TextureInfo texInfo;
        texInfo.filename = TEST_IMAGE;
        texInfo.tintColor = tintColor;
        logic->ET_setTextureInfo(texInfo);
    }

    Vec2 quadSize = Vec2(256.f);
    logic->ET_setSize(quadSize);
    logic->ET_show();
 
    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    syncWithGameAndDrawToImageBuffer();

    {
        AABB2Di box;
        box.setCenterAndSize(getRenderResolution() / 2, Vec2i(256));
        EXPECT_EQ(0, countNotEqualPixels(box, tintColor));
    }

    tm.scale = Vec3(2.f);
    logic->setTransform(tm);

    syncWithGameAndDrawToImageBuffer();

    {
        AABB2Di box;
        box.setCenterAndSize(getRenderResolution() / 2, Vec2i(512));
        EXPECT_EQ(0, countNotEqualPixels(box, tintColor));
    }
}

TEST_F(RenderCmdTexturedQuadTests, CheckLinearGradient) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderLinearGradientRect>();

    Vec2 quadSize = Vec2(256.f);
    logic->ET_setSize(quadSize);
    logic->ET_show();
 
    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    syncWithGameAndDrawToImageBuffer();

    {
        auto clearColor = getClearColor();
        AABB2Di box;
        box.setCenterAndSize(getRenderResolution() / 2, Vec2i(256));
        EXPECT_EQ(0, countEqualPixels(box, clearColor));
    }
}
