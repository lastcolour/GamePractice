#include "UILabelTests.hpp"
#include "Logics/UILabel.hpp"
#include "Logics/RenderTextLogic.hpp"

UILabel* UILabelTests::createUILabel() {
    auto entity = createVoidObject();
    auto uiLabelPtr = new UILabel;
    entity->addCustomLogic(std::unique_ptr<EntityLogic>(uiLabelPtr));
    entity->addCustomLogic(std::unique_ptr<EntityLogic>(new RenderTextLogic));
    uiLabelPtr->ET_setTextRender(entity->getEntityId());
    return uiLabelPtr;
}

TEST_F(UILabelTests, CheckTextSize) {
    auto label = createUILabel();
    label->ET_setText("");

    {
        auto box = label->ET_getBox();
        auto size = box.getSize();
        ASSERT_EQ(size.x, 0);
        ASSERT_EQ(size.y, 0);
    }

    label->ET_setText("YyPpQq");

    label->ET_setFontSize(15.f);

    auto smallBox = label->ET_getBox();
    auto smallSize = smallBox.getSize();
    {
        ASSERT_GT(smallSize.x, 0);
        ASSERT_GT(smallSize.y, 0);
    }

    label->ET_setFontSize(30.f);

    auto bigBox = label->ET_getBox();
    auto bigSize = bigBox.getSize();
    {
        ASSERT_GE(bigSize.x, smallSize.x * 2);
        ASSERT_GE(bigSize.y, smallSize.y * 2);
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

        EXPECT_EQ(origSize.x, scaledSize.x * 2);
        EXPECT_EQ(origSize.y, scaledSize.y * 2);
    }

    tm.scale *= 2;

    ET_SendEvent(label->getEntityId(), &ETEntity::ET_setTransform, tm);

    {
        auto origSize = origBox.getSize();
        auto scaledSize = scaledBox.getSize();

        EXPECT_EQ(origSize.x, scaledSize.x * 2);
        EXPECT_EQ(origSize.y, scaledSize.y * 2);
    }
}