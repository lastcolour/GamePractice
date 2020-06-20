#include "UIViewTests.hpp"
#include "Logics/UIView.hpp"
#include "Render/ETRenderCamera.hpp"

TEST_F(UIViewTests, CheckViewSize) {
    auto view = createObjectAndLogic<UIView>();
    ASSERT_TRUE(view->init());

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    {
        auto box = view->ET_getBox();

        auto boxSize = box.getSize();
        EXPECT_EQ(boxSize.x, renderPort.x);
        EXPECT_EQ(boxSize.y, renderPort.y);

        auto boxCenter = box.getCenter();
        EXPECT_EQ(boxCenter.x, renderPort.x / 2);
        EXPECT_EQ(boxCenter.y, renderPort.y / 2);
    }

    renderPort = renderPort / 2;
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, renderPort);
    ET_SendEvent(&ETRenderCameraEvents::ET_onRenderPortResized);

    {
        auto box = view->ET_getBox();

        auto boxSize = box.getSize();
        EXPECT_EQ(boxSize.x, renderPort.x);
        EXPECT_EQ(boxSize.y, renderPort.y);

        auto boxCenter = box.getCenter();
        EXPECT_EQ(boxCenter.x, renderPort.x / 2);
        EXPECT_EQ(boxCenter.y, renderPort.y / 2);
    }
}