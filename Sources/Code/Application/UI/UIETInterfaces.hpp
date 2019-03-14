#ifndef __UI_ET_INTERFACES_HPP__
#define __UI_ET_INTERFACES_HPP__

#include "Math/AABB.hpp"
#include "UI/UIStyle.hpp"

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual AABB2Di ET_getParentAaabb2di() const = 0;
    virtual const AABB2Di& ET_getAaabb2di() const = 0;
    virtual void ET_setCenter(const Vec2i& pos) = 0;
    virtual void ET_alignInBox(const AABB2Di& alignBox) = 0;
};

struct ETUIButton {
    virtual ~ETUIButton() = default;
    virtual void ET_onPress() = 0;
};

struct ETUIList {
    virtual ~ETUIList() = default;
    virtual void ET_addElement(EntityId newElemId) = 0;
};

#endif /* __UI_ET_INTERFACES_HPP__ */