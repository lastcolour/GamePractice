#include "UILabelTests.hpp"
#include "Logics/UILabel.hpp"
#include "Logics/RenderTextLogic.hpp"

UILabel* UILabelTests::createUILabel() {
    auto entity = createVoidObject();
    HACK_ASSERT_TRUE(entity->addCustomLogic<RenderTextLogic>());
    auto uiLabelPtr = entity->addCustomLogic<UILabel>();
    HACK_ASSERT_TRUE(uiLabelPtr);
    return uiLabelPtr;
}

TEST_F(UILabelTests, CheckTextSize) {
    auto label = createUILabel();
    label->ET_setText("");

    {
        auto box = label->ET_getBox();
        auto size = box.getSize();
        ASSERT_FLOAT_EQ(size.x, 0.f);
        ASSERT_FLOAT_EQ(size.y, 0.f);
    }

    label->ET_setText("YyPpQq");

    label->ET_setFontSize(15.f);

    auto smallBox = label->ET_getBox();
    auto smallSize = smallBox.getSize();
    {
        ASSERT_GT(smallSize.x, 0.f);
        ASSERT_GT(smallSize.y, 0.f);
    }

    label->ET_setFontSize(30.f);

    auto bigBox = label->ET_getBox();
    auto bigSize = bigBox.getSize();
    {
        ASSERT_GE(bigSize.x, smallSize.x * 2.f);
        ASSERT_GE(bigSize.y, smallSize.y * 2.f);
    }
}

TEST_F(UILabelTests, CheckTextScaling) {
    auto label = createUILabel();
    label->ET_setText("Start Game");

    auto origBox = label->ET_getBox();

    Transform tm;
    ET_SendEventReturn(tm, label->getEntityId(), &ETEntity::ET_getTransform);

    tm.scale *= 0.5f;

    ET_SendEvent(label->getEntityId(), &ETEntity::ET_setTransform, tm);

    auto scaledBox = label->ET_getBox();

    {
        auto origSize = origBox.getSize();
        auto scaledSize = scaledBox.getSize();

        EXPECT_FLOAT_EQ(origSize.x, 2.f * scaledSize.x);
        EXPECT_FLOAT_EQ(origSize.y, 2.f * scaledSize.y);
    }

    tm.scale *= 2.f;

    ET_SendEvent(label->getEntityId(), &ETEntity::ET_setTransform, tm);
    auto rescaledBox = label->ET_getBox();

    {
        auto origSize = origBox.getSize();
        auto rescaledSize = rescaledBox.getSize();

        EXPECT_FLOAT_EQ(origSize.x, rescaledSize.x);
        EXPECT_FLOAT_EQ(origSize.y, rescaledSize.y);
    }
}