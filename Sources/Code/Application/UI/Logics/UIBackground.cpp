#include "UI/Logics/UIBackground.hpp"
#include "Entity/ETEntityInterfaces.hpp"

UIBackground::UIBackground() {
}

UIBackground::~UIBackground() {
}

bool UIBackground::serialize(const JSONNode& node) {
    return true;
}

bool UIBackground::init() {
    ETNode<ETRenderEvents>::connect(getEntityId());
    ET_onRenderPortResized();
    return true;
}

void UIBackground::ET_onRenderPortResized() {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    ET_SendEvent(getEntityId(), &ETRenderRect::ET_setSize, renderPort);

    Transform tm;
    tm.pt = Vec3(renderPort.x * 0.5f, renderPort.y * 0.5f, 0.f);
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
}