#include "RenderUtilsTests.hpp"
#include "RenderUtils.hpp"

TEST_F(RenderUtilsTests, CheckGetNinePatchVertexCoord_UpScale_0) {
    Vec2i imageSize = Vec2i(10, 10);
    Vec2 patches = Vec2(0.3f, 0.3f);
    float patchScale = 0.f;

    Vec2 drawSizes[] = {
        Vec2(10.f), Vec2(20.f), Vec2(40.f, 20.f), Vec2(20.f, 40.f)
    };

    for(int i = 0; i < sizeof(drawSizes) / sizeof(Vec2); ++i) {
        auto& drawSize = drawSizes[i];

        auto res = RenderUtils::GetNinePatchVertexCoord(imageSize, drawSize, patches, patchScale);

        EXPECT_FLOAT_EQ(imageSize.x * patches.x, drawSize.x * res.x);
        EXPECT_FLOAT_EQ(imageSize.y * patches.y, drawSize.y * res.y);
    }
}

TEST_F(RenderUtilsTests, CheckGetNinePatchVertexCoord_UpScale_1) {
    Vec2i imageSize = Vec2i(10, 10);
    Vec2 patches = Vec2(0.3f);
    float patchScale = 1.f;

    Vec2 drawSizes[] = {
        Vec2(10.f), Vec2(20.f), Vec2(40.f, 20.f), Vec2(20.f, 40.f)
    };

    for(int i = 0; i < sizeof(drawSizes) / sizeof(Vec2); ++i) {
        auto& drawSize = drawSizes[i];

        auto res = RenderUtils::GetNinePatchVertexCoord(imageSize, drawSize, patches, patchScale);

        EXPECT_FLOAT_EQ(drawSize.x * res.x, drawSize.y * res.y);
    }
}

TEST_F(RenderUtilsTests, CheckGetNinePatchVertexCoord_UpScale_10) {
    Vec2i imageSize = Vec2i(10, 10);
    Vec2 patches = Vec2(0.3f);
    float patchScale = 10.f;

    Vec2 drawSizes[] = {
        Vec2(10.f), Vec2(20.f), Vec2(40.f, 20.f), Vec2(20.f, 40.f)
    };

    for(int i = 0; i < sizeof(drawSizes) / sizeof(Vec2); ++i) {
        auto& drawSize = drawSizes[i];

        auto res = RenderUtils::GetNinePatchVertexCoord(imageSize, drawSize, patches, patchScale);

        EXPECT_FLOAT_EQ(drawSize.x * res.x, drawSize.y * res.y);
    }
}


TEST_F(RenderUtilsTests, CheckGetNinePatchVertexCoord_DownScale_0) {
    Vec2i imageSize = Vec2i(10, 10);
    Vec2 patches = Vec2(0.3f, 0.3f);
    float patchScale = 0.f;

    Vec2 drawSizes[] = {
        Vec2(5.f), Vec2(2.f), Vec2(5.f, 2.f), Vec2(2.f, 5.f)
    };

    for(int i = 0; i < sizeof(drawSizes) / sizeof(Vec2); ++i) {
        auto& drawSize = drawSizes[i];

        auto res = RenderUtils::GetNinePatchVertexCoord(imageSize, drawSize, patches, patchScale);

        float m = std::max(res.x, res.y);

        EXPECT_FLOAT_EQ(m, 0.5f);
        EXPECT_FLOAT_EQ(res.x * drawSize.x, res.y * drawSize.y);
    }
}