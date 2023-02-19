#include "RenderTests/RenderCmdColorQuadTests.hpp"
#include "Render/src/Commands/DrawColoredQuadExecutor.hpp"
#include "Render/src/Logics/RenderSimpleLogic.hpp"
#include "Render/src/RenderUtils.hpp"
#include "Math/MatrixTransform.hpp"

TEST_F(RenderCmdColorQuadTests, CheckCreateCmd) {
    DrawColoredQuadExecutor executor;
    EXPECT_TRUE(executor.init());

    auto cmd = static_cast<DrawColoredQuadCmd*>(executor.createCmd());
    EXPECT_TRUE(cmd);

    const ColorB drawColor(255, 255, 0);
    const Vec2 quadSize(100.f);

    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    cmd->modelMat = tm.toMat4();
    Math::AddScale3D(cmd->modelMat, Vec3(quadSize.x / 2.f, quadSize.y / 2.f, 1.f));
    cmd->color = drawColor;
    cmd->visible = true;

    EXPECT_TRUE(DrawColoredQuadCmd::IsVisible(*cmd));

    RenderState renderState;
    EXPECT_TRUE(renderState.init());

    executor.registerCmdForDraw(cmd);
    executor.preDraw(renderState);

    auto slice = executor.getCmdSlice();

    EXPECT_EQ(slice.size, 1u);
    EXPECT_EQ(slice.startIdx, 0u);

    slice.advance();
    EXPECT_EQ(slice.size, 1u);
    EXPECT_EQ(slice.startIdx, 0u);
    EXPECT_EQ(slice.endIdx, 1u);

    renderState.mainFBO->bind();
    ASSERT_TRUE(renderState.mainFBO->resize(getRenderResolution()));
    renderState.mainFBO->unbind();

    Math::ProjectOrtho(renderState.proj2dMat, 0.f, static_cast<float>(getRenderResolution().x),
        0.f, static_cast<float>(getRenderResolution().y), -1.f, 1.f);

    RENDER_FB->bind();
    executor.draw(renderState, slice);
    RENDER_FB->unbind();

    RenderUtils::ReadFramebufferToImage(*RENDER_FB, *IMAGE_BUFFER);

    {
        Vec2i start;
        start.x = static_cast<int>(getRenderResolution().x / 2.f - quadSize.x / 2.f);
        start.y = static_cast<int>(getRenderResolution().y / 2.f - quadSize.y / 2.f);

        Vec2i end;
        end.x = static_cast<int>(getRenderResolution().x / 2.f + quadSize.x / 2.f);
        end.y = static_cast<int>(getRenderResolution().y / 2.f + quadSize.y / 2.f);
        auto failPixCount = countNotEqualPixels(start, end, drawColor);
        EXPECT_EQ(failPixCount, 0);
    }

    slice.advance();
    EXPECT_TRUE(slice.empty());
}

TEST_F(RenderCmdColorQuadTests, CheckSimpleLogic) {
    auto entity = createVoidObject();
    auto simpleLogic = entity->addCustomLogic<RenderSimpleLogic>();

    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    const ColorB drawColor(255, 255, 0);
    const Vec2 quadSize(100.f);

    simpleLogic->ET_show();
    simpleLogic->ET_setSize(quadSize);
    simpleLogic->ET_setColor(drawColor);

    syncWithGameAndDrawToImageBuffer();
    {
        Vec2i start;
        start.x = static_cast<int>(getRenderResolution().x / 2.f - quadSize.x / 2.f);
        start.y = static_cast<int>(getRenderResolution().y / 2.f - quadSize.y / 2.f);

        Vec2i end;
        end.x = static_cast<int>(getRenderResolution().x / 2.f + quadSize.x / 2.f);
        end.y = static_cast<int>(getRenderResolution().y / 2.f + quadSize.y / 2.f);
        auto failPixCount = countNotEqualPixels(start, end, drawColor);
        EXPECT_EQ(failPixCount, 0);
    }

    simpleLogic->ET_hide();
    syncWithGameAndDrawToImageBuffer();
    {
        Vec2i start;
        start.x = static_cast<int>(getRenderResolution().x / 2.f - quadSize.x / 2.f);
        start.y = static_cast<int>(getRenderResolution().y / 2.f - quadSize.y / 2.f);

        Vec2i end;
        end.x = static_cast<int>(getRenderResolution().x / 2.f + quadSize.x / 2.f);
        end.y = static_cast<int>(getRenderResolution().y / 2.f + quadSize.y / 2.f);
        auto failPixCount = countNotEqualPixels(start, end, drawColor);
        EXPECT_GE(failPixCount, (end.x - start.x - 3) * (end.y - start.y - 3));
    }
}

TEST_F(RenderCmdColorQuadTests, CheckStencil) {
    // TOOD
}

TEST_F(RenderCmdColorQuadTests, CheckAlpha) {
    // TOOD
}