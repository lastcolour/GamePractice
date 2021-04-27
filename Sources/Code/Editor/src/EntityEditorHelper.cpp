#include "EntityEditorHelper.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Render/ETRenderNode.hpp"

namespace {

const int DRAW_ALPHA = 180;

AABB2D getRenderRect(const Transform& tm, EntityId entId) {
    float normScale = 1.f;
    ET_SendEventReturn(normScale, entId, &ETRenderNode::ET_getNormalizationScale);

    AABB2D box(0.f);
    ET_SendEventReturn(box.top, entId, &ETRenderRect::ET_getSize);
    box.top.x *= tm.scale.x * normScale;
    box.top.y *= tm.scale.y * normScale;
    box.setCenter(tm.pt.x, tm.pt.y);

    return box;
}

void drawUIElementHelp(const Transform& tm, EntityId entId) {
    bool isHidden = false;
    ET_SendEventReturn(isHidden, entId, &ETUIElement::ET_isHidden);
    if(isHidden) {
        return;
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, entId, &ETUIElement::ET_getBox);
    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(125, 255, 255, DRAW_ALPHA));

    UIBoxMargin margin;
    ET_SendEventReturn(margin, entId, &ETUIElement::ET_getMargin);

    box.bot.x -= margin.left;
    box.bot.y -= margin.bot;
    box.top.x += margin.right;
    box.top.y += margin.top;

    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(255, 125, 125, DRAW_ALPHA));
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
        box.setCenter(tm.pt.x, tm.pt.y);
        ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(255, 255, 245, DRAW_ALPHA));
    } else {
        float r = normScale * emissionConfig.emitterVal.x;
        r *= tm.scale.x;
        ET_SendEvent(&ETDebugRender::ET_drawCicleBorder, Vec2(tm.pt.x, tm.pt.y), r, ColorB(255, 255, 245, DRAW_ALPHA));
    }
}

void drawRenderRectHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    auto box = getRenderRect(tm, entId);
    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(127, 252, 3, DRAW_ALPHA));
}

void drawRenderTextHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, entId, &ETRenderTextLogic::ET_getTextAABB);

    ET_SendEvent(&ETDebugRender::ET_drawQuadBorder, box, ColorB(127, 252, 3, DRAW_ALPHA));
}

void drawNinePatchImageHelp(const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    Vec2 vertCoord(0.f);
    ET_SendEventReturn(vertCoord, entId, &ETNinePatchImageLogic::ET_getPatchesVertCoord);

    auto box = getRenderRect(tm, entId);
    auto size = box.getSize();

    Vec2 p1(0.f);
    Vec2 p2(0.f);
    {
        p1.x = size.x * vertCoord.x;
        p1.y = 0.f;
        p2.x = p1.x;
        p2.y = size.y;
        p1 += box.bot;
        p2 += box.bot;
        ET_SendEvent(&ETDebugRender::ET_drawLine, p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
    }
    {
        p1.x = size.x * (1.f - vertCoord.x);
        p1.y = 0.f;
        p2.x = p1.x;
        p2.y = size.y;
        p1 += box.bot;
        p2 += box.bot;
        ET_SendEvent(&ETDebugRender::ET_drawLine, p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
    }
    {
        p1.x = 0.f;
        p1.y = size.y * vertCoord.y;
        p2.x = size.x;
        p2.y = p1.y;
        p1 += box.bot;
        p2 += box.bot;
        ET_SendEvent(&ETDebugRender::ET_drawLine, p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
    }
    {
        p1.x = 0.f;
        p1.y = size.y * (1.f - vertCoord.y);
        p2.x = size.x;
        p2.y = p1.y;
        p1 += box.bot;
        p2 += box.bot;
        ET_SendEvent(&ETDebugRender::ET_drawLine, p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
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
    if(ET_IsExistNode<ETNinePatchImageLogic>(focusEntId)) {
        drawNinePatchImageHelp(tm, focusEntId);
    }
}