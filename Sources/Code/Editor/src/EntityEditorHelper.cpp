#include "EntityEditorHelper.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Render/ETRenderNode.hpp"

namespace {

const int DRAW_ALPHA = 180;

void drawUIElementHelp(const Transform& tm, EntityId entId) {
    bool isHidden = false;
    ET_SendEventReturn(isHidden, entId, &ETUIElement::ET_isHidden);
    if(isHidden) {
        return;
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, entId, &ETUIElement::ET_getBox);
    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(125, 255, 255, DRAW_ALPHA), 1.f);

    UIBoxMargin margin;
    ET_SendEventReturn(margin, entId, &ETUIElement::ET_getMargin);

    box.bot.x -= margin.left;
    box.bot.y -= margin.bot;
    box.top.x += margin.right;
    box.top.y += margin.top;

    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(255, 125, 125, DRAW_ALPHA), 0.75f);
}

void drawParticlesHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    float normScale = 1.f;
    ET_SendEventReturn(normScale, entId, &ETRenderNode::ET_getNormalizationScale);

    ParticlesEmitterEmissionConfig emissionConfig;
    ET_SendEventReturn(emissionConfig, entId, &ETParticlesSystem::ET_getEmissionConfig);

    if(emissionConfig.emitterType == EmitterType::Box) {
        AABB2D box(0.f);
        box.top = 2.f * normScale * emissionConfig.emitterVal;
        box.top.scale(Vec2(tm.scale.x, tm.scale.y));
        box.setCenter(Vec2(tm.pt.x, tm.pt.y));
        ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(255, 255, 245, DRAW_ALPHA), 1.f);
    } else {
        float r = normScale * emissionConfig.emitterVal.x;
        r *= tm.scale.x;
        ET_SendEvent(&ETDebugRender::ET_drawCicleBorder, Vec2(tm.pt.x, tm.pt.y), r, ColorB(255, 255, 245, DRAW_ALPHA), 1.f);
    }
}

void drawRenderRectHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    float normScale = 1.f;
    ET_SendEventReturn(normScale, entId, &ETRenderNode::ET_getNormalizationScale);

    AABB2D box(0.f);
    ET_SendEventReturn(box.top, entId, &ETRenderRect::ET_getSize);
    box.top *= normScale;
    box.setCenter(Vec2(tm.pt.x, tm.pt.y));

    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(127, 252, 3, DRAW_ALPHA), 1.f);
}

void drawRenderTextHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, entId, &ETRenderTextLogic::ET_getTextAABB);

    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(127, 252, 3, DRAW_ALPHA), 1.f);
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