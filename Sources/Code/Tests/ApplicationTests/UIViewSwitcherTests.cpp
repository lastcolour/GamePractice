#include "UIViewSwitcherTests.hpp"
#include "TestUtils/UITestUtils.hpp"

#include <UI/UIViewSwitcher.hpp>
#include <UI/Logics/UIBaseBox.hpp>
#include <Entity/Entity.hpp>

namespace {

void CheckExpectedCenter(UIBaseBox* uiBox, const Vec2i& expectedCenter) {
    auto& box = uiBox->ET_getAabb2di();
    auto center = box.getCenter();

    EXPECT_EQ(center.x, expectedCenter.x);
    EXPECT_EQ(center.y, expectedCenter.y);
}

} // namespace

TEST_F(UISwitcherTests, CheckViewSwtich) {
    UIBaseBox* firstView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());

    float swtichDuration = switcher.ET_getTotalSwitchDuration();
    ASSERT_GT(swtichDuration, 0.f);

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
        ET_SendEventReturn(tm, firstView->getEntityId(), &ETEntity::ET_getTransform);
        Vec2i tmPt = Vec2i(static_cast<int>(tm.pt.x), static_cast<int>(tm.pt.y));
        ASSERT_EQ(tmPt, renderPort / 2);
    }
}

TEST_F(UISwitcherTests, CheckSwtichEndDuringResize) {
    UIBaseBox* firstView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());


    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());
    float swtichDuration = switcher.ET_getTotalSwitchDuration();

    ET_SendEvent(&ETRenderEvents::ET_onRenderPortResized);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    {
        auto firstBox = firstView->ET_getAabb2di();
        auto center = firstBox.getCenter();
        auto expectedCenter = renderPort / 2;
        EXPECT_EQ(center, expectedCenter);
    }

    {
        auto secondBox = secondView->ET_getAabb2di();
        auto center = secondBox.getCenter();
        auto expectedCenter = Vec2i( - renderPort.x / 2, renderPort.y / 2);
        EXPECT_EQ(center, expectedCenter);
    }
}

TEST_F(UISwitcherTests, CheckNoramlSwitchAndReverse) {

    UIBaseBox* firstView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());
    float swtichDuration = switcher.ET_getTotalSwitchDuration();

    switcher.ET_onTick(swtichDuration / 4.f);

    switcher.ET_reverse();

    {
        CheckExpectedCenter(firstView, Vec2i(renderPort.x * 5 / 4, renderPort.y / 2));
        CheckExpectedCenter(secondView, Vec2i(renderPort.x / 4, renderPort.y / 2));
    }

    switcher.ET_onTick(swtichDuration / 4.f);

    {
        CheckExpectedCenter(firstView, Vec2i(renderPort.x * 3 / 2, renderPort.y / 2));
        CheckExpectedCenter(secondView, Vec2i(renderPort.x / 2, renderPort.y / 2));
    }

    switcher.ET_onTick(swtichDuration / 2.f);

    {
        CheckExpectedCenter(firstView, Vec2i(renderPort.x * 3 / 2, renderPort.y / 2));
        CheckExpectedCenter(secondView, Vec2i(renderPort.x / 2, renderPort.y / 2));
    }
}

TEST_F(UISwitcherTests, CheckReverseSwitch) {

    UIBaseBox* firstView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    switcher.ET_reverseSwitchView(firstView->getEntityId(), secondView->getEntityId());
    float swtichDuration = switcher.ET_getTotalSwitchDuration();

    {
        CheckExpectedCenter(firstView, Vec2i(-renderPort.x / 2, renderPort.y / 2));
        CheckExpectedCenter(secondView, Vec2i(renderPort.x / 2, renderPort.y / 2));
    }

    switcher.ET_onTick(swtichDuration / 2.f);

    {
        CheckExpectedCenter(firstView, Vec2i(0, renderPort.y / 2));
        CheckExpectedCenter(secondView, Vec2i(renderPort.x, renderPort.y / 2));
    }

    switcher.ET_onTick(swtichDuration / 2.f);

    {
        CheckExpectedCenter(firstView, Vec2i(renderPort.x / 2, renderPort.y / 2));
        CheckExpectedCenter(secondView, Vec2i(renderPort.x * 3 / 2, renderPort.y / 2));
    }
}

TEST_F(UISwitcherTests, CheckAppearOnTop) {

}