#include "UIImageTests.hpp"
#include "UI/Logics/UIImage.hpp"

namespace {

const char* TEST_IMAGE = "Images/options.png";

} // namespace

TEST_F(UIImageTests, CheckUIImageConnections) {
    auto uiImage = createObjectAndLogic<UIImage>();
    ASSERT_TRUE(uiImage->init());

    EXPECT_TRUE(ET_IsExistNode<ETUIBox>(uiImage->getEntityId()));
    EXPECT_TRUE(ET_IsExistNode<ETUIImage>(uiImage->getEntityId()));
}

TEST_F(UIImageTests, CheckEmptyImage) {
    auto uiImage = createObjectAndLogic<UIImage>();
    ASSERT_TRUE(uiImage->init());

    auto imageBox = uiImage->ET_getAabb2di();
    auto imageSize = imageBox.getSize();
    auto imageCenter = imageBox.getCenter();

    EXPECT_EQ(imageSize.x, 0);
    EXPECT_EQ(imageSize.y, 0);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    EXPECT_EQ(imageCenter.x, renderPort.x / 2);
    EXPECT_EQ(imageCenter.y, renderPort.y / 2);
}

TEST_F(UIImageTests, CheckSimpleImage) {
    auto uiImage = createObjectAndLogic<UIImage>();
    uiImage->ET_setImage(TEST_IMAGE);
    ASSERT_TRUE(uiImage->init());

    auto imageBox = uiImage->ET_getAabb2di();
    auto imageSize = imageBox.getSize();

    EXPECT_GT(imageSize.x, 0);
    EXPECT_GT(imageSize.y, 0);
}