#include "Logics/UIView.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIViewScript.hpp"

void UIView::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIView>("UIView")) {
        classInfo->addBaseClass<UIBox>();
    }
}

UIView::UIView() :
    hasFocus(false) {
}

UIView::~UIView() {
}

void UIView::ET_setFocus(bool flag) {
    if(flag == hasFocus) {
        return;
    }
    hasFocus = flag;
    if(hasFocus) {
        ET_SendEvent(getEntityId(), &ETUIElement::ET_enable);
        ET_SendEvent(getEntityId(), &ETUIViewScript::ET_onViewGetFocus);
    } else {
        ET_SendEvent(getEntityId(), &ETUIElement::ET_disable);
        ET_SendEvent(getEntityId(), &ETUIViewScript::ET_onViewLostFocus);
    }
}

bool UIView::ET_getFocus() const {
    return hasFocus;
}

void UIView::init() {
    UIBox::init();

    ETNode<ETUIView>::connect(getEntityId());

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    ET_onViewPortChanged(viewPort);
}

void UIView::ET_onViewPortChanged(const Vec2i& newSize) {
    UIBox::ET_onViewPortChanged(newSize);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt.x = newSize.x / 2.f;
    tm.pt.y = newSize.y / 2.f;
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);

    ET_setLayoutPos(Vec2i(0));
}