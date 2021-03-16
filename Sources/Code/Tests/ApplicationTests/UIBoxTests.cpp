#include "UIBoxTests.hpp"
#include "Logics/UIBox.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Logics/UILayout.hpp"
#include "Core/GlobalData.hpp"

namespace {

void CheckBoxSize(const AABB2D& box, const Vec2i& exSize) {
    auto size = box.getSize();
    EXPECT_FLOAT_EQ(size.x, static_cast<float>(exSize.x));
    EXPECT_FLOAT_EQ(size.y, static_cast<float>(exSize.y));
}

void CheckBoxCenter(const AABB2D box, const Vec2i& exCenter) {
    auto center = box.getSize();
    EXPECT_FLOAT_EQ(center.x, static_cast<float>(exCenter.x));
    EXPECT_FLOAT_EQ(center.y, static_cast<float>(exCenter.y));
}

} // namespace

void UIBoxTests::SetUp() {
    ConsoleAppTests::SetUp();
    auto gridSize = GetGlobal<UIConfig>()->horizontalGrid;
    Vec2i portSize(gridSize, gridSize);
    ET_SendEvent(&ETRenderCameraEvents::ET_onRenderPortResized, portSize);
}

UIBox* UIBoxTests::createUIBox() {
    auto object = createVoidObject();
    UIBox* uiBox = object->addCustomLogic<UIBox>();
    HACK_ASSERT_TRUE(uiBox);
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
        style.height = GetGlobal<UIConfig>()->horizontalGrid / 2.f;
        style.heightInv = UIBoxSizeInvariant::Grid;
        uiBox->ET_setStyle(style);

        auto aabb = uiBox->ET_getBox();
        CheckBoxSize(aabb, Vec2i(viewPort.x / 4, viewPort.y / 2));
        CheckBoxCenter(aabb, viewPort / 2);
    }
    {
        UIBoxStyle style;
        style.width = GetGlobal<UIConfig>()->horizontalGrid / 2.f;
        style.widthInv = UIBoxSizeInvariant::Grid;
        style.height = 0.25f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);

        auto aabb = uiBox->ET_getBox();
        CheckBoxSize(aabb, Vec2i(viewPort.x / 2, viewPort.y / 4));
        CheckBoxCenter(aabb, viewPort / 2);
    }
}

TEST_F(UIBoxTests, CheckViewPortResized) {
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

    ET_SendEvent(&ETUIViewPort::ET_setViewPort, viewPort);

    {
        auto aabb = uiBox->ET_getBox();
        CheckBoxSize(aabb, Vec2i(viewPort.x / 2, viewPort.y / 2));
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
        CheckBoxSize(aabb, Vec2i(viewPort.x / 2, viewPort.y / 2));
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
        CheckBoxSize(aabb, Vec2i(viewPort.x / 2, viewPort.y / 2));
        CheckBoxCenter(aabb, Vec2i(viewPort.x / 2, viewPort.y / 2));
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
        CheckBoxSize(aabb, Vec2i(viewPort.x / 2, viewPort.y / 2));
        CheckBoxCenter(aabb, Vec2i(viewPort.x / 4, viewPort.y / 4));
    }
}

TEST_F(UIBoxTests, CheckParentAlphaAffectChild) {
    auto parentEntity = createVoidObject();
    parentEntity->addCustomLogic<UIBox>();
    parentEntity->addCustomLogic<UILayout>();

    float parentAlpha = 0.7f;
    ET_SendEvent(parentEntity->getEntityId(), &ETUIElement::ET_setAlpha, parentAlpha);

    auto childEntity = createVoidObject();
    childEntity->addCustomLogic<UIBox>();

    float childAlpha = 0.7f;
    ET_SendEvent(childEntity->getEntityId(), &ETUIElement::ET_setAlpha, childAlpha);

    ET_SendEvent(parentEntity->getEntityId(), &ETUILayout::ET_addItem, childEntity->getEntityId());

    {
        float resChildAlpha = 0.f;
        ET_SendEventReturn(resChildAlpha, childEntity->getEntityId(), &ETUIElement::ET_getAlpha);
        EXPECT_FLOAT_EQ(resChildAlpha, parentAlpha * childAlpha);
    }

    parentAlpha = 0.1f;
    ET_SendEvent(parentEntity->getEntityId(), &ETUIElement::ET_setAlpha, parentAlpha);

    {
        float resChildAlpha = 0.f;
        ET_SendEventReturn(resChildAlpha, childEntity->getEntityId(), &ETUIElement::ET_getAlpha);
        EXPECT_FLOAT_EQ(resChildAlpha, parentAlpha * childAlpha);
    }
}