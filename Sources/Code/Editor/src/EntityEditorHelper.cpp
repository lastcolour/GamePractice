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

void drawUIElementHelp(DebugInfoDrawer& dd, const Transform& tm, EntityId entId) {
    bool isHidden = false;
    ET_SendEventReturn(isHidden, entId, &ETUIElement::ET_isHidden);
    if(isHidden) {
        return;
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, entId, &ETUIElementGeom::ET_getBox);
    dd.drawQuadBorder(box, ColorB(125, 255, 255, DRAW_ALPHA));

    UIBoxMargin margin;
    ET_SendEventReturn(margin, entId, &ETUIElementGeom::ET_getMargin);

    box.bot.x -= margin.left;
    box.bot.y -= margin.bot;
    box.top.x += margin.right;
    box.top.y += margin.top;

    dd.drawQuadBorder(box, ColorB(255, 125, 125, DRAW_ALPHA));
}

void drawParticlesHelp(DebugInfoDrawer& dd, const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    float normScale = 1.f;
    ET_SendEventReturn(normScale, entId, &ETRenderNode::ET_getNormalizationScale);

    ParticlesEmitterEmissionConfig emissionConf;
    ET_SendEventReturn(emissionConf, entId, &ETParticlesSystem::ET_getEmissionConfig);

    if(emissionConf.emitterType == EmitterType::Box) {
        AABB2D box(0.f);
        box.top = 2.f * normScale * emissionConf.emitterVal;
        box.top.scale(Vec2(tm.scale.x, tm.scale.y));
        box.setCenter(tm.pt.x, tm.pt.y);
        dd.drawQuadBorder(box, ColorB(255, 255, 245, DRAW_ALPHA));
    } else {
        float r = normScale * emissionConf.emitterVal.x;
        r *= tm.scale.x;
        dd.drawCircleBorder(Vec2(tm.pt.x, tm.pt.y), r, ColorB(255, 255, 245, DRAW_ALPHA));
    }

    ParticlesEmitterMovementConfig movementConf;
    ET_SendEventReturn(movementConf, entId, &ETParticlesSystem::ET_getMovementConfig);

    auto dir = Vec2(cos(Math::Deg2Rad(emissionConf.direction)),
        sin(Math::Deg2Rad(emissionConf.direction)));

    {
        auto v = Vec3(dir, 0.f);
        v = tm.quat * v;
        dir = Vec2(v.x, v.y);
    }

    float speed = normScale * movementConf.speed * emissionConf.lifetime;

    Vec2 start(tm.pt.x, tm.pt.y);
    {
        auto end = start + speed * Vec2(dir.x * tm.scale.x, dir.y * tm.scale.y);
        dd.drawLine(start, end, ColorB(240, 100, 3, DRAW_ALPHA));
    }

    {
        dd.drawCircleArc(start, speed * tm.scale.x, dir,
            2.f * Math::Deg2Rad(emissionConf.directionVar), ColorB(127, 252, 3, DRAW_ALPHA));
    }
}

void drawRenderRectHelp(DebugInfoDrawer& dd, const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    auto box = getRenderRect(tm, entId);
    dd.drawQuadBorder(box, ColorB(127, 252, 3, DRAW_ALPHA));
}

void drawRenderTextHelp(DebugInfoDrawer& dd, const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, entId, &ETRenderTextLogic::ET_getTextAABB);

    dd.drawQuadBorder(box, ColorB(127, 252, 3, DRAW_ALPHA));
}

void drawNinePatchImageHelp(DebugInfoDrawer& dd, const Transform& tm, EntityId entId) {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, entId, &ETRenderNode::ET_isVisible);
    if(!isVisible) {
        return;
    }

    Vec2 vertCoord(0.f);
    ET_SendEventReturn(vertCoord, entId, &ETNinePatchImageLogic::ET_getPatchesVertCoord);
    vertCoord /= 2.f;

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
        dd.drawLine(p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
    }
    {
        p1.x = size.x * (1.f - vertCoord.x);
        p1.y = 0.f;
        p2.x = p1.x;
        p2.y = size.y;
        p1 += box.bot;
        p2 += box.bot;
        dd.drawLine(p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
    }
    {
        p1.x = 0.f;
        p1.y = size.y * vertCoord.y;
        p2.x = size.x;
        p2.y = p1.y;
        p1 += box.bot;
        p2 += box.bot;
        dd.drawLine(p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
    }
    {
        p1.x = 0.f;
        p1.y = size.y * (1.f - vertCoord.y);
        p2.x = size.x;
        p2.y = p1.y;
        p1 += box.bot;
        p2 += box.bot;
        dd.drawLine(p1, p2, ColorB(127, 252, 3, DRAW_ALPHA));
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

void EntityEditorHelper::ET_drawDebugInfo(DebugInfoDrawer& dd) {
    if(!focusEntId.isValid()) {
        return;
    }
    Transform tm;
    ET_SendEventReturn(tm, focusEntId, &ETEntity::ET_getTransform);

    if(ET_IsExistNode<ETUIElement>(focusEntId)) {
        drawUIElementHelp(dd, tm, focusEntId);
    }
    if(ET_IsExistNode<ETParticlesSystem>(focusEntId)) {
        drawParticlesHelp(dd, tm, focusEntId);
    }
    if(ET_IsExistNode<ETRenderRect>(focusEntId)) {
        drawRenderRectHelp(dd, tm, focusEntId);
    }
    if(ET_IsExistNode<ETRenderTextLogic>(focusEntId)) {
        drawRenderTextHelp(dd, tm, focusEntId);
    }
    if(ET_IsExistNode<ETNinePatchImageLogic>(focusEntId)) {
        drawNinePatchImageHelp(dd, tm, focusEntId);
    }
}