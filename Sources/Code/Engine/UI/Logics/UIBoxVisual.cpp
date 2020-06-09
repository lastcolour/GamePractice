#include "UI/Logics/UIBoxVisual.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

void UIBoxVisual::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIBoxVisual>("UIBoxVisual")) {
        classInfo->addField("render", &UIBoxVisual::renderId);
    }
}

UIBoxVisual::UIBoxVisual() {
}

UIBoxVisual::~UIBoxVisual() {
}

bool UIBoxVisual::init() {
    ET_onBoxResized();
    ETNode<ETUIBoxEvents>::connect(getEntityId());
    ETNode<ETUIVisibleElement>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    return true;
}

void UIBoxVisual::deinit() {
}

void UIBoxVisual::ET_show() {
}

void UIBoxVisual::ET_hide() {
}

bool UIBoxVisual::ET_isVisible() const {
    return true;
}

int UIBoxVisual::ET_getZIndex() const {
    return 0;
}

void UIBoxVisual::ET_setZIndex(int newZIndex) {
}

void UIBoxVisual::ET_onBoxResized() {
    if(!renderId.isValid()) {
        return;
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(renderId, &ETEntity::ET_setTransform, tm);

    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getBox);
    ET_SendEvent(renderId, &ETRenderRect::ET_setSize, box.getSize());
}

void UIBoxVisual::ET_onTransformChanged(const Transform& newTm) {
    ET_onBoxResized();
}