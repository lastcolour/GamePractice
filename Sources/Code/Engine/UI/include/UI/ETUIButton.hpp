#ifndef __ET_UI_BUTTON_HPP__
#define __ET_UI_BUTTON_HPP__

#include "Math/AABB.hpp"
#include "Platform/ETSurface.hpp"

enum class EInputEventResult {
    Accept = 0,
    Ignore
};

struct ETUIInteractionBox {
    virtual ~ETUIInteractionBox() = default;
    virtual EInputEventResult ET_onInputEvent(EActionType type, const Vec2i& pt) = 0;
    virtual AABB2Di ET_getHitBox() const = 0;
};

struct ETUIButtonEventManager {
    virtual ~ETUIButtonEventManager() = default;
    virtual EntityId ET_getActiveButton() const = 0;
    virtual void ET_setActiveButton(EntityId buttonId) = 0;
};

#endif /* __ET_UI_BUTTON_HPP__ */