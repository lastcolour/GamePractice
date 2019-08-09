#include "UIViewSwitcherTests.hpp"

#include <Game/ETGameInterfaces.hpp>
#include <UI/UIViewSwitcher.hpp>
#include <UI/Logics/UIBaseBox.hpp>
#include <Game/GameObject.hpp>

UIBaseBox* UISwitcherTests::createView() {
    auto object = createVoidObject();
    std::unique_ptr<UIBaseBox> uiBaseBoxPtr(new UIBaseBox);
    UIBaseBox* uiBaseBox = uiBaseBoxPtr.get();
    object->addLogic(std::move(uiBaseBoxPtr));
    tempObject.push_back(std::move(object));
    return uiBaseBox;
}

TEST_F(UISwitcherTests, CheckViewSwtich) {
    UIBaseBox* firstView = createView();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createView();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    float swtichDuration = switcher.ET_getSwitchDuration();
    ASSERT_GT(swtichDuration, 0.f);

    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    {
        auto firstBox = firstView->ET_getAabb2di();
        auto center = firstBox.getCenter();
        auto expectedCenter = Vec2i(renderPort.x * 3 / 2, renderPort.y / 2);
        EXPECT_EQ(center, expectedCenter);

        auto secondBox = secondView->ET_getAabb2di();
        center = secondBox.getCenter();
        expectedCenter = renderPort / 2;
        EXPECT_EQ(center, expectedCenter);
    }

    switcher.ET_onTick(swtichDuration / 2.f);

    {
        auto firstBox = firstView->ET_getAabb2di();
        auto center = firstBox.getCenter();
        auto expectedCenter = Vec2i(renderPort.x, renderPort.y / 2);
        EXPECT_EQ(center, expectedCenter);

        auto secondBox = secondView->ET_getAabb2di();
        center = secondBox.getCenter();
        expectedCenter = Vec2i(0, renderPort.y / 2);
        EXPECT_EQ(center, expectedCenter);
    }

    switcher.ET_onTick(swtichDuration / 2.f + 0.1f);

    {
        auto firstBox = firstView->ET_getAabb2di();
        auto center = firstBox.getCenter();
        auto expectedCenter = renderPort / 2;
        EXPECT_EQ(center, expectedCenter);

        auto secondBox = secondView->ET_getAabb2di();
        center = secondBox.getCenter();
        expectedCenter = Vec2i( - renderPort.x / 2, renderPort.y / 2);
        EXPECT_EQ(center, expectedCenter);
    }

    {
        Transform tm;
        ET_SendEventReturn(tm, firstView->getEntityId(), &ETGameObject::ET_getTransform);
        Vec2i tmPt = Vec2i(static_cast<int>(tm.pt.x), static_cast<int>(tm.pt.y));
        ASSERT_EQ(tmPt, renderPort / 2);
    }
}

TEST_F(UISwitcherTests, CheckSwtichEndDuringResize) {
    ASSERT_TRUE(false);
}