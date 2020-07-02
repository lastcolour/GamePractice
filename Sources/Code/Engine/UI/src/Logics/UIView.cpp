#include "Logics/UIView.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Reflect/ReflectContext.hpp"

void UIView::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIView>("UIView")) {
        classInfo->addBaseClass<UIBox>();
    }
}

UIView::UIView() {
}

UIView::~UIView() {
}

bool UIView::init() {
    UIBox::init();
    ET_onRenderPortResized();
    return true;
}

void UIView::ET_onRenderPortResized() {
    UIBox::ET_onRenderPortResized();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt.x = renderPort.x / 2.f;
    tm.pt.y = renderPort.y / 2.f;
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
}