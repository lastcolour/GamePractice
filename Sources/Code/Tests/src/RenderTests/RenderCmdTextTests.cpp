#include "RenderTests/RenderCmdTextTests.hpp"
#include "Render/src/Logics/RenderTextLogic.hpp"
#include "Render/ETRenderManager.hpp"

TEST_F(RenderCmdTextTests, DrawSingleLetter) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderTextLogic>();

    logic->ET_setText("A");
    logic->ET_setFontHeight(24.f);
    logic->ET_setColor(ColorB(255, 255, 255));
    logic->ET_show();

    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    auto textBox = logic->ET_getTextAABB();

    EXPECT_GE(textBox.getSize().x, 10.f);
    EXPECT_GE(textBox.getSize().y, 10.f);

    syncWithGameAndDrawToImageBuffer();

    AABB2Di itextBox = AABB2Di{Vec2i(static_cast<int>(textBox.bot.x), static_cast<int>(textBox.bot.y)), 
        Vec2i(static_cast<int>(textBox.top.x), static_cast<int>(textBox.top.y))};

    auto pixelsInBounds = countEqualPixels(itextBox, ColorB(255, 255, 255));
    EXPECT_GT(pixelsInBounds, 0);

    auto biggerBox = itextBox.expandBy(100);

    auto pixelsOutOfBounds = countEqualPixels(biggerBox, ColorB(255, 255, 255));

    EXPECT_EQ(pixelsOutOfBounds, pixelsOutOfBounds);
}

TEST_F(RenderCmdTextTests, DrawMultiLineText) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderTextLogic>();

    logic->ET_setText("Hello\nWorld!");
    logic->ET_setFontHeight(24.f);
    logic->ET_setColor(ColorB(255, 255, 255));
    logic->ET_show();

    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    auto textBox = logic->ET_getTextAABB();

    EXPECT_GE(textBox.getSize().x, 10.f);
    EXPECT_GE(textBox.getSize().y, 10.f);

    syncWithGameAndDrawToImageBuffer();

    AABB2Di itextBox = AABB2Di{Vec2i(static_cast<int>(textBox.bot.x), static_cast<int>(textBox.bot.y)), 
        Vec2i(static_cast<int>(textBox.top.x), static_cast<int>(textBox.top.y))};

    auto pixelsInBounds = countEqualPixels(itextBox, ColorB(255, 255, 255));
    EXPECT_GT(pixelsInBounds, 0);

    auto biggerBox = itextBox.expandBy(100);

    auto pixelsOutOfBounds = countEqualPixels(biggerBox, ColorB(255, 255, 255));

    EXPECT_EQ(pixelsOutOfBounds, pixelsOutOfBounds);
}

TEST_F(RenderCmdTextTests, DrawEmptyText) {
    auto entity = createVoidObject();
    auto logic = entity->addCustomLogic<RenderTextLogic>();

    logic->ET_setText("   \n\t    \t   \n    \n");
    logic->ET_setFontHeight(24.f);
    logic->ET_setColor(ColorB(255, 255, 255));
    logic->ET_show();

    Transform tm;
    tm.pt = Vec3(getRenderResolution().x / 2.f, getRenderResolution().y / 2.f, 0.f);
    entity->ET_setTransform(tm);

    auto textBox = logic->ET_getTextAABB();

    EXPECT_GE(textBox.getSize().x, 10.f);
    EXPECT_GE(textBox.getSize().y, 10.f);

    syncWithGameAndDrawToImageBuffer();

    AABB2Di itextBox = AABB2Di{Vec2i(static_cast<int>(textBox.bot.x), static_cast<int>(textBox.bot.y)), 
        Vec2i(static_cast<int>(textBox.top.x), static_cast<int>(textBox.top.y))};

    auto pixelsInBounds = countEqualPixels(itextBox, ColorB(255, 255, 255));
    EXPECT_EQ(pixelsInBounds, 0);

    auto biggerBox = itextBox.expandBy(100);

    auto pixelsOutOfBounds = countEqualPixels(biggerBox, ColorB(255, 255, 255));

    EXPECT_EQ(pixelsOutOfBounds, pixelsOutOfBounds);
}

TEST_F(RenderCmdTextTests, CheckCreateSameFontTwice) {
    std::shared_ptr<RenderFont> font1;
    ET_SendEventReturn(font1, &ETRenderFontManager::ET_createFont, EFontType::Game);

    ASSERT_TRUE(font1);

    auto atlas = font1->getFontAtlas();
    ASSERT_TRUE(atlas);
    ASSERT_GT(atlas->size, Vec2i(0));
    ASSERT_NE(atlas->texId, 0u);

    std::shared_ptr<RenderFont> font2;
    ET_SendEventReturn(font2, &ETRenderFontManager::ET_createFont, EFontType::Game);

    ASSERT_EQ(font1.get(), font2.get());
}

TEST_F(RenderCmdTextTests, CheckTextBoxCorrespondDrawBox) {
    auto gameObj = createVoidObject();
    RenderTextLogic* renderText = gameObj->addCustomLogic<RenderTextLogic>();
    ASSERT_TRUE(renderText);

    renderText->ET_setFontHeight(48.f);

    Vec2i renderRes = getRenderResolution();

    Vec2 center;
    center.x = renderRes.x / 2.f;
    center.y = renderRes.y / 2.f;

    Transform tm;
    tm.pt = Vec3(center, 0.f);
    gameObj->ET_setTransform(tm);

    const int FAIL_PIX_TOL = 4;

    std::vector<std::string> words = {"a", "A", "aA", "1", "1 1", "p", "pP", "Yy", "a\na", "a\nPp"};
    for(const auto& word : words) {
        renderText->ET_setText(word.c_str());
        auto box = renderText->ET_getTextAABB();

        syncWithGameAndDrawToImageBuffer();
        auto dirtyBox = getDirtyArea();

        {
            auto dirtyBoxSize = dirtyBox.getSize();
            auto boxSize = box.getSize();

            EXPECT_GE(boxSize.x, dirtyBoxSize.x - FAIL_PIX_TOL) << "word='" << word << "'";
            EXPECT_LE(boxSize.x, dirtyBoxSize.x + FAIL_PIX_TOL) << "word='" << word << "'";

            EXPECT_GE(boxSize.y, dirtyBoxSize.y - FAIL_PIX_TOL) << "word='" << word << "'";
            EXPECT_LE(boxSize.y, dirtyBoxSize.y + FAIL_PIX_TOL) << "word='" << word << "'";
        }

        if(word.empty()) {
            EXPECT_EQ(dirtyBox.bot.x, 0);
            EXPECT_EQ(dirtyBox.bot.y, 0);
            EXPECT_EQ(dirtyBox.top.x, 0);
            EXPECT_EQ(dirtyBox.top.y, 0);
        } else {
            EXPECT_GE(box.bot.x, dirtyBox.bot.x - FAIL_PIX_TOL) << "word='" << word << "'";
            EXPECT_LE(box.bot.x, dirtyBox.bot.x + FAIL_PIX_TOL) << "word='" << word << "'";

            EXPECT_GE(box.bot.y, dirtyBox.bot.y - FAIL_PIX_TOL) << "word='" << word << "'";
            EXPECT_LE(box.bot.y, dirtyBox.bot.y + FAIL_PIX_TOL) << "word='" << word << "'";

            EXPECT_GE(box.top.x, dirtyBox.top.x - FAIL_PIX_TOL) << "word='" << word << "'";
            EXPECT_LE(box.top.x, dirtyBox.top.x + FAIL_PIX_TOL) << "word='" << word << "'";

            EXPECT_GE(box.top.y, dirtyBox.top.y - FAIL_PIX_TOL) << "word='" << word << "'";
            EXPECT_LE(box.top.y, dirtyBox.top.y + FAIL_PIX_TOL) << "word='" << word << "'";
        }
    }
}