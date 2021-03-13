#include "EntityEditorHelper.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Render/ETRenderNode.hpp"

namespace {

void drawUIElementHelp(const Transform& tm, EntityId entId) {
    bool isHidden = false;
    ET_SendEventReturn(isHidden, entId, &ETUIElement::ET_isHidden);
    if(isHidden) {
        return;
    }

    AABB2Di box(0);
    ET_SendEventReturn(box, entId, &ETUIElement::ET_getBox);

    AABB2D quad(0.f);
    quad.bot = Vec2(static_cast<float>(box.bot.x), static_cast<float>(box.bot.y));
    quad.top = Vec2(static_cast<float>(box.top.x), static_cast<float>(box.top.y));
    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, quad, ColorB(125, 255, 255), 1.f);
}

void drawParticlesHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }
}

void drawRenderRectHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }
}

void drawRenderTextHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }
}

} // namespace

EntityEditorHelper::EntityEditorHelper() {
}

EntityEditorHelper::~EntityEditorHelper() {
}

bool EntityEditorHelper::init() {
    ETNode<ETEntityEditorHelper>::connect(getEntityId());
    ETNode<ETDebugInfoProvider>::connect(getEntityId());
    return true;
}

void EntityEditorHelper::deinit() {
}

void EntityEditorHelper::ET_setFocusEntity(EntityId newFocusEntId) {
    focusEntId = newFocusEntId;
}

void EntityEditorHelper::ET_drawDebugInfo() {
    if(!focusEntId.isValid()) {
        return;
    }
    Transform tm;
    ET_SendEventReturn(tm, focusEntId, &ETEntity::ET_getTransform);

    if(ET_IsExistNode<ETUIElement>(focusEntId)) {
        drawUIElementHelp(tm, focusEntId);
    }
    if(ET_IsExistNode<ETParticlesSystem>(focusEntId)) {
        drawParticlesHelp(tm, focusEntId);
    }
    if(ET_IsExistNode<ETRenderRect>(focusEntId)) {
        drawRenderRectHelp(tm, focusEntId);
    }
    if(ET_IsExistNode<ETRenderTextLogic>(focusEntId)) {
        drawRenderTextHelp(tm, focusEntId);
    }
}