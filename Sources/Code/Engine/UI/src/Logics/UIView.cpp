#include "Logics/UIView.hpp"
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

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    ET_onViewPortChanged(viewPort);

    return true;
}

void UIView::ET_onViewPortChanged(const Vec2i& newSize) {
    UIBox::ET_onViewPortChanged(newSize);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt.x = newSize.x / 2.f;
    tm.pt.y = newSize.y / 2.f;
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
}