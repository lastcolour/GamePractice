#include "RenderTests/RenderCmdNinePatchTests.hpp"
#include "Render/src/RenderUtils.hpp"
#include "Render/src/Logics/NinePatchImageLogic.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderManager.hpp"

TEST_F(RenderCmdNinePatchTests, PatchCalc_NoScale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {300.f, 400.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.3f);
    EXPECT_FLOAT_EQ(res.y, 0.3f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_X_Upscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {600.f, 400.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.15f);
    EXPECT_FLOAT_EQ(res.y, 0.3f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_Y_Upscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {300.f, 800.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.3f);
    EXPECT_FLOAT_EQ(res.y, 0.15f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_XY_Upscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {600.f, 800.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.15f);
    EXPECT_FLOAT_EQ(res.y, 0.15f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_X_Downscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {150.f, 400.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.4999f);
    EXPECT_FLOAT_EQ(res.y, 0.3f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_Y_Downscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {300.f, 200.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.3f);
    EXPECT_FLOAT_EQ(res.y, 0.4999f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_XY_Downscale_Uniform) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {150.f, 200.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.4999f);
    EXPECT_FLOAT_EQ(res.y, 0.4999f);
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_XY_Downscale_NonUniform) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {100.f, 100.f};
    Vec2 patches = {0.3f, 0.3f};
    Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patches);

    EXPECT_FLOAT_EQ(res.x, 0.374925f);
    EXPECT_FLOAT_EQ(res.y, 0.4999f);
}

TEST_F(RenderCmdNinePatchTests, RenderNinePatch) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<NinePatchImageLogic>();

    Vec2i renderPort = getRenderResolution();

    Transform tm;
    tm.pt = Vec3(renderPort.x / 2.f, renderPort.y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    Vec2 drawSize = {renderPort.x / 2.f, renderPort.y / 2.f};

    logic->ET_setSize(drawSize);
    logic->ET_setPatches(Vec2(0.333f));

    syncWithGame();

    std::shared_ptr<RenderTexture> tex;
    {
        ET_SendEventReturn(tex, &ETRenderTextureManager::ET_createTexture, ETextureDataType::RGBA, Vec2i(3));
        ASSERT_TRUE(tex);

        tex->bind();

        ColorB texData[] = {
            ColorB(255, 0, 0), ColorB(0, 255, 0), ColorB(255, 0, 0),
            ColorB(0, 255, 0), ColorB(0, 0, 255), ColorB(0, 255, 0),
            ColorB(255, 0, 0), ColorB(0, 255, 0), ColorB(255, 0, 0),
        };

        tex->writeRGBA(Vec2i(0), tex->getSize(), &texData[0]);
        tex->unbind();

        auto NinePathDrawCmd = static_cast<DrawNinePatchCmd*>(logic->getDrawCmd());
        NinePathDrawCmd->texInfo = TextureInfo{};
        NinePathDrawCmd->texInfo.lerpType = ETextureLerpType::Point;
        NinePathDrawCmd->texObj = tex;

        NinePathDrawCmd->updateVertCoords();
    }

    logic->ET_show();

    syncWithGameAndDrawToImageBuffer();

    Vec2i center = renderPort / 2;
    Vec2i size = {static_cast<int>(drawSize.x), static_cast<int>(drawSize.y)};

    int cornerSquareSize = size.y / 3;

    Vec2i a0 = center - size / 2;
    Vec2i a1 = Vec2i(a0.x + cornerSquareSize, a0.y);
    Vec2i a2 = Vec2i(a0.x + size.x - cornerSquareSize, a0.y);
    Vec2i a3 = Vec2i(a0.x + size.x, a0.y);

    Vec2i b0 = Vec2i(a0.x, a0.y + cornerSquareSize);
    Vec2i b1 = Vec2i(a1.x, a1.y + cornerSquareSize);
    Vec2i b2 = Vec2i(a2.x, a2.y + cornerSquareSize);
    Vec2i b3 = Vec2i(a3.x, a3.y + cornerSquareSize);

    Vec2i c0 = Vec2i(b0.x, b0.y + cornerSquareSize);
    Vec2i c1 = Vec2i(b1.x, b1.y + cornerSquareSize);
    Vec2i c2 = Vec2i(b2.x, b2.y + cornerSquareSize);
    Vec2i c3 = Vec2i(b3.x, b3.y + cornerSquareSize);

    Vec2i d0 = Vec2i(c0.x, c0.y + cornerSquareSize);
    Vec2i d1 = Vec2i(c1.x, c1.y + cornerSquareSize);
    Vec2i d2 = Vec2i(c2.x, c2.y + cornerSquareSize);
    Vec2i d3 = Vec2i(c3.x, c3.y + cornerSquareSize);

    {
        AABB2Di box(a0, b1);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(255, 0, 0)));
    }

    {
        AABB2Di box(c0, d1);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(255, 0, 0)));
    }

    {
        AABB2Di box(a2, b3);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);
        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(255, 0, 0)));
    }

    {
        AABB2Di box(c2, d3);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);
        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(255, 0, 0)));
    }

    {
        AABB2Di box(a1, b2);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);
        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(0, 255, 0)));
    }

    {
        AABB2Di box(b0, c1);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(0, 255, 0)));
    }

    {
        AABB2Di box(b2, c3);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(0, 255, 0)));
    }

    {
        AABB2Di box(c1, d2);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(0, 255, 0)));
    }

    {
        AABB2Di box(b1, c2);
        box.bot += Vec2i(1);
        box.top -= Vec2i(1);

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(255, 255, 0)));
    }
}