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

void UIScrollFocus::ET_setFocusToEntity(EntityId focusEntId) {
    Transform tm;
    ET_SendEventReturn(tm, focusEntId, &ETEntity::ET_getTransform);
    Vec2i foucsPt(tm.pt.x, tm.pt.y);

    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    Vec2i currAreaPt(tm.pt.x, tm.pt.y);

    EntityId scrollTargetId;
    ET_SendEventReturn(scrollTargetId, getEntityId(), &ETUIScrollArea::ET_getTarget);

    ET_SendEventReturn(tm, scrollTargetId, &ETEntity::ET_getTransform);
    Vec2i currTargetPt(tm.pt.x, tm.pt.y);

    Vec2i resPt = currTargetPt - foucsPt + currAreaPt;

    ET_SendEvent(getEntityId(), &ETUIScrollArea::ET_setTargetPosClamped, resPt);
}

void UIScrollFocus::ET_onUITick(float dt) {
}