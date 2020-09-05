#include "UIBoxTests.hpp"
#include "Logics/UIBox.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Render/ETRenderCamera.hpp"

void UIBoxTests::SetUp() {
    ConsoleAppTests::SetUp();
    auto gridSize = ET_getShared<UIConfig>()->horizontalGrid;
    Vec2i portSize(gridSize, gridSize);
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, portSize);
}

UIBox* UIBoxTests::createUIBox() {
    auto object = createVoidObject();
    std::unique_ptr<UIBox> uiBoxPtr(new UIBox);
    UIBox* uiBox = uiBoxPtr.get();
    object->addCustomLogic(std::move(uiBoxPtr));
    return uiBox;
}

TEST_F(UIBoxTests, CheckUIBoxSize) {
    UIBox* uiBox = createUIBox();

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    Transform tm;
    tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
    ET_SendEvent(uiBox->getEntityId(), &ETEntity::ET_setTransform, tm);

    {
        UIBoxStyle style;
        style.width = 0.25f;
        style.widthInv = UIBoxSizeInvariant::Relative;
        style.height = ET_getShared<UIConfig>()->horizontalGrid / 2.f;
        style.heightInv = UIBoxSizeInvariant::Grid;
        uiBox->ET_setStyle(style);

        auto aabb = uiBox->ET_getBox();
        ASSERT_EQ(aabb.getSize(), Vec2i(viewPort.x / 4, viewPort.y / 2));
        ASSERT_EQ(aabb.getCenter(), viewPort / 2);
    }
    {
        UIBoxStyle style;
        style.width = ET_getShared<UIConfig>()->horizontalGrid / 2.f;
        style.widthInv = UIBoxSizeInvariant::Grid;
        style.height = 0.25f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);

        auto aabb = uiBox->ET_getBox();
        ASSERT_EQ(aabb.getSize(), Vec2i(viewPort.x / 2, viewPort.y / 4));
        ASSERT_EQ(aabb.getCenter(), viewPort / 2);
    }
}

TEST_F(UIBoxTests, CheckRenderPortResized) {
    auto uiBox = createUIBox();

    UIBoxStyle style;
    style.width = 0.5f;
    style.widthInv = UIBoxSizeInvariant::Relative;
    style.height = 0.5f;
    style.heightInv = UIBoxSizeInvariant::Relative;
    uiBox->ET_setStyle(style);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    viewPort.x *= 2;
    viewPort.y *= 2;

    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, viewPort);

    {
        auto aabb = uiBox->ET_getBox();
        ASSERT_EQ(aabb.getSize(), Vec2i(viewPort.x / 2, viewPort.y / 2));
    }
}

TEST_F(UIBoxTests, CheckTransformScaleChanged) {
    auto uiBox = createUIBox();
    {
        UIBoxStyle style;
        style.width = 1.f;
        style.widthInv = UIBoxSizeInvariant::Relative;
        style.height = 1.f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);
    }
    {
        Transform tm;
        ET_SendEventReturn(tm, uiBox->getEntityId(), &ETEntity::ET_getTransform);
        tm.scale = Vec3(0.5f, 0.5f, 1.f);

        ET_SendEvent(uiBox->getEntityId(), &ETEntity::ET_setTransform, tm);
    }
    {
        Vec2i viewPort(0);
        ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

        auto aabb = uiBox->ET_getBox();
        ASSERT_EQ(aabb.getSize(), Vec2i(viewPort.x / 2, viewPort.y / 2));
    }
}

TEST_F(UIBoxTests, CheckTransformPosChanged) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto uiBox = createUIBox();
    {
        Transform tm;
        tm.pt = Vec3(static_cast<float>(viewPort.x) / 2.f,
            static_cast<float>(viewPort.y) / 2.f, 0.f);

        ET_SendEvent(uiBox->getEntityId(), &ETEntity::ET_setTransform, tm);
    }
    {
        UIBoxStyle style;
        style.width = 0.5f;
        style.widthInv = UIBoxSizeInvariant::Relative;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);
    }
    {
        auto aabb = uiBox->ET_getBox();
        ASSERT_EQ(aabb.getCenter(), Vec2i(viewPort.x / 2, viewPort.y / 2));
        ASSERT_EQ(aabb.getSize(),  Vec2i(viewPort.x / 2, viewPort.y / 2));
    }
    {
        Transform tm;
        ET_SendEventReturn(tm, uiBox->getEntityId(), &ETEntity::ET_getTransform);
        tm.pt = Vec3(static_cast<float>(viewPort.x) / 4.f,
            static_cast<float>(viewPort.y) / 4.f, 0.f);

        ET_SendEvent(uiBox->getEntityId(), &ETEntity::ET_setTransform, tm);
    }
    {
        auto aabb = uiBox->ET_getBox();
        ASSERT_EQ(aabb.getCenter(), Vec2i(viewPort.x / 4, viewPort.y / 4));
        ASSERT_EQ(aabb.getSize(),  Vec2i(viewPort.x / 2, viewPort.y / 2));
    }
}