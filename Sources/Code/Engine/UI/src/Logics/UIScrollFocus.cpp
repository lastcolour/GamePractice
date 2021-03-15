#include "Logics/UIScrollFocus.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIBox.hpp"
#include "Math/AABB.hpp"
#include "Entity/ETEntity.hpp"
#include "Core/ETLogger.hpp"

void UIScrollFocus::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIScrollFocus>("UIScrollFocus")) {
    }
}

UIScrollFocus::UIScrollFocus() {
}

UIScrollFocus::~UIScrollFocus() {
}

void UIScrollFocus::init() {
    ETNode<ETUIScrollFocus>::connect(getEntityId());
}

void UIScrollFocus::deinit() {
}

void UIScrollFocus::ET_setPendingFocus() {
    if(!focusEntId.isValid()) {
        return;
    }
    bool isHidden = false;
    ET_SendEventReturn(isHidden, getEntityId(), &ETUIElement::ET_isHidden);
    if(isHidden) {
        LogError("[UIScrollFocus::ET_setPendingFocus] Can't set focus to entity: '%s' if UI element: '%s' is hidden",
            EntityUtils::GetEntityName(focusEntId), EntityUtils::GetEntityName(getEntityId()));
        focusEntId = InvalidEntityId;
        return;
    }
    applyFocusToEntity();
}

void UIScrollFocus::ET_setFocusToEntity(EntityId newFocusEntId) {
    bool isHidden = false;
    focusEntId = newFocusEntId;
    ET_SendEventReturn(isHidden, getEntityId(), &ETUIElement::ET_isHidden);
    if(isHidden) {
        return;
    }
    applyFocusToEntity();
}

void UIScrollFocus::applyFocusToEntity() {
    Transform tm;
    ET_SendEventReturn(tm, focusEntId, &ETEntity::ET_getTransform);
    Vec2 foucsPt(tm.pt.x, tm.pt.y);

    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    Vec2 currAreaPt(tm.pt.x, tm.pt.y);

    EntityId scrollTargetId;
    ET_SendEventReturn(scrollTargetId, getEntityId(), &ETUIScrollArea::ET_getTarget);

    ET_SendEventReturn(tm, scrollTargetId, &ETEntity::ET_getTransform);
    Vec2 currTargetPt(tm.pt.x, tm.pt.y);

    Vec2 resPt = currTargetPt - foucsPt + currAreaPt;

    ET_SendEvent(getEntityId(), &ETUIScrollArea::ET_setTargetPosClamped, resPt);

    focusEntId = InvalidEntityId;
}

void UIScrollFocus::ET_onUITick(float dt) {
}