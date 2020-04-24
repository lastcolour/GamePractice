#include "UI/Logics/UIAppearFromSide.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Math/Transform.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>
#include <cassert>

UIAppearFromSide::UIAppearFromSide() :
    animDuration(0.5f),
    viewScale(0.8f),
    animState(AnimState::Init),
    isAppearing(true) {
}

UIAppearFromSide::~UIAppearFromSide() {
}

bool UIAppearFromSide::serialize(const JSONNode& node) {
    node.read("animDuration", animDuration);
    return true;
}

bool UIAppearFromSide::init() {
    ETNode<ETUIAppearAnimation>::connect(getEntityId());
    return true;
}

void UIAppearFromSide::ET_setAppear(bool flag) {
    isAppearing = flag;
}

bool UIAppearFromSide::ET_isAppearing() const {
    return isAppearing;
}

bool UIAppearFromSide::ET_animate(float duration) {
    switch(animState)
    {
    case AnimState::Init: {
        initAnimation();
        if(animState != AnimState::Animating) {
            break;
        }
    }
    case AnimState::Animating: {
        updateAnimation(duration);
        if(animState != AnimState::Done) {
            break;
        }
    }
    case AnimState::Done: {
        endAnimation();
        return true;
    }
    default:
        assert(false && "Invalid anim state");
        break;
    }
    return false;
}

void UIAppearFromSide::initAnimation() {
    if(isAppearing) {
        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
        
        Vec2i renderSize;
        ET_SendEventReturn(renderSize, &ETRenderCamera::ET_getRenderPort);

        tm.pt.x += static_cast<float>(renderSize.x);

        ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
    }

    animState = AnimState::Animating;
}

void UIAppearFromSide::updateAnimation(float duration) {
    Vec2i renderSize;
    ET_SendEventReturn(renderSize, &ETRenderCamera::ET_getRenderPort);

    Vec2 targetPt(renderSize.x / 2.f, renderSize.y / 2.f);
    Vec2 startPt = targetPt;
    float startScale = 1.f;
    float targetScale = startScale;
    if(isAppearing) {
        startPt.x += renderSize.x;
        startScale = 0.8f;
    } else {
        targetPt.x += renderSize.x;
        startScale = 0.8f;
        targetScale = 1.f;
    }

    float prog = std::min(duration / animDuration, 1.f);
    auto resPt = Math::Lerp(startPt, targetPt, prog);
    auto resScale = Math::Lerp(startScale, targetScale, prog);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt.x = resPt.x;
    tm.pt.y = resPt.y;
    tm.scale = Vec3(resScale);
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);

    if(duration > animDuration) {
        animState = AnimState::Done;
    }
}

void UIAppearFromSide::endAnimation() {
    animState = AnimState::Init;
}

bool UIAppearFromSide::ET_isNeedHideOldView() const {
    return true;
}

float UIAppearFromSide::ET_getDuration() const {
    return animDuration;
}
