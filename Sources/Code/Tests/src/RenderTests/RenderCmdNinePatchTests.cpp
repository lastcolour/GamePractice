#include "RenderTests/RenderCmdNinePatchTests.hpp"
#include "Render/src/RenderUtils.hpp"
#include "Render/src/Logics/NinePatchImageLogic.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderManager.hpp"

TEST_F(RenderCmdNinePatchTests, PatchCalc_NoScale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {300.f, 400.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.6f);
        EXPECT_FLOAT_EQ(res.y, 0.45f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.8f);
        EXPECT_FLOAT_EQ(res.y, 0.6f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.13333334f);
        EXPECT_FLOAT_EQ(res.y, 0.1f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_X_Upscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {600.f, 400.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.4f);
        EXPECT_FLOAT_EQ(res.y, 0.6f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.6f);
        EXPECT_FLOAT_EQ(res.y, 0.9f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.06666667f);
        EXPECT_FLOAT_EQ(res.y, 0.1f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_Y_Upscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {300.f, 800.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.6f);
        EXPECT_FLOAT_EQ(res.y, 0.225f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.99980003f);
        EXPECT_FLOAT_EQ(res.y, 0.37499249f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.13333334f);
        EXPECT_FLOAT_EQ(res.y, 0.050000001f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_XY_Upscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {600.f, 800.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.6f);
        EXPECT_FLOAT_EQ(res.y, 0.45f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.80000001f);
        EXPECT_FLOAT_EQ(res.y, 0.6f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.06666667f);
        EXPECT_FLOAT_EQ(res.y, 0.050000001f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_X_Downscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {150.f, 400.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.6f);
        EXPECT_FLOAT_EQ(res.y, 0.22500001f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.99980003f);
        EXPECT_FLOAT_EQ(res.y, 0.37499249f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.26666668f);
        EXPECT_FLOAT_EQ(res.y, 0.1f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_Y_Downscale) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {300.f, 200.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.40000004f);
        EXPECT_FLOAT_EQ(res.y, 0.60000002f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.6f);
        EXPECT_FLOAT_EQ(res.y, 0.90000004f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.13333334f);
        EXPECT_FLOAT_EQ(res.y, 0.2f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_XY_Downscale_Uniform) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {150.f, 200.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.99980003f);
        EXPECT_FLOAT_EQ(res.y, 0.99980003f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.99980003f);
        EXPECT_FLOAT_EQ(res.y, 0.99980003f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.99980003f);
        EXPECT_FLOAT_EQ(res.y, 0.99980003f);
    }
}

TEST_F(RenderCmdNinePatchTests, PatchCalc_XY_Downscale_NonUniform) {
    Vec2i imgSize = {300, 400};
    Vec2 drawSize = {100.f, 100.f};
    float patchSize = 0.3f;

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::SmallestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.74985003f);
        EXPECT_FLOAT_EQ(res.y, 0.99980003f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, patchSize, ENinePatchSizeType::BiggestSidePct);

        EXPECT_FLOAT_EQ(res.x, 0.74985003f);
        EXPECT_FLOAT_EQ(res.y, 0.99980003f);
    }

    {
        Vec2 res = RenderUtils::GetNinePatchVertexCoord(imgSize, drawSize, 20.f, ENinePatchSizeType::Pixels);

        EXPECT_FLOAT_EQ(res.x, 0.74985003f);
        EXPECT_FLOAT_EQ(res.y, 0.99980003f);
    }
}

TEST_F(RenderCmdNinePatchTests, RenderNinePatch) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<NinePatchImageLogic>();

    Vec2i renderPort = getRenderResolution();

    Transform tm;
    tm.pt = Vec3(renderPort.x / 2.f, renderPort.y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    Vec2 drawSize(std::min(renderPort.x, renderPort.y) * 3 / 10);

    logic->ET_setSize(drawSize);
    logic->ET_setPatchSize(drawSize.x / 3.f);
    logic->ET_setPatchSizeType(ENinePatchSizeType::Pixels);
    logic->ET_setPatchesTextureCoords(Vec2(0.333f));

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

    int cornerSquareSize = size.x / 3;

    Vec2i a0 = center - size / 2;
    Vec2i a1 = Vec2i(a0.x + cornerSquareSize, a0.y);
    Vec2i a2 = Vec2i(a0.x + size.x - cornerSquareSize, a0.y);
    Vec2i a3 = Vec2i(a0.x + size.x, a0.y);

    Vec2i b0 = Vec2i(a0.x, a0.y + cornerSquareSize);
    Vec2i b1 = Vec2i(a1.x, a0.y + cornerSquareSize);
    Vec2i b2 = Vec2i(a2.x, a0.y + cornerSquareSize);
    Vec2i b3 = Vec2i(a3.x, a0.y + cornerSquareSize);

    Vec2i c0 = Vec2i(a0.x, a0.y + size.y - cornerSquareSize);
    Vec2i c1 = Vec2i(a1.x, a0.y + size.y - cornerSquareSize);
    Vec2i c2 = Vec2i(a2.x, a0.y + size.y - cornerSquareSize);
    Vec2i c3 = Vec2i(a3.x, a0.y + size.y - cornerSquareSize);

    Vec2i d0 = Vec2i(a0.x, a0.y + size.y);
    Vec2i d1 = Vec2i(a1.x, a0.y + size.y);
    Vec2i d2 = Vec2i(a2.x, a0.y + size.y);
    Vec2i d3 = Vec2i(a3.x, a0.y + size.y);

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

        EXPECT_EQ(0u, countNotEqualPixels(box, ColorB(0, 0, 255)));
    }
}