#include "UI/Logics/UIView.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

void UIView::Reflect(ReflectContext& ctx) {
    ctx.classInfo<UIView>("UIView");
}

UIView::UIView() {
}

UIView::~UIView() {
}

bool UIView::init() {
    UIBoxStyle viewStyle;
    viewStyle.width = 1.f;
    viewStyle.widthInv = UIBoxSizeInvariant::Relative;
    viewStyle.height = 1.f;
    viewStyle.heightInv = UIBoxSizeInvariant::Relative;

    viewStyle.margin.bot = 0.f;
    viewStyle.margin.top = 0.f;
    viewStyle.margin.left = 0.f;
    viewStyle.margin.right = 0.f;

    UIBox::ET_setStyle(viewStyle);
    ET_onRenderPortResized();
    return UIBox::init();
}

void UIView::ET_onRenderPortResized() {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt.x = renderPort.x / 2.f;
    tm.pt.y = renderPort.y / 2.f;
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
}