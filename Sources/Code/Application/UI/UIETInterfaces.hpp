#ifndef __UI_ET_INTERFACES_HPP__
#define __UI_ET_INTERFACES_HPP__

#include "Math/AABB.hpp"
#include "UI/UIStyle.hpp"

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual const AABB2Di& ET_getAaabb2di() const = 0;
    virtual void ET_setUIListPos(int pos) = 0;
};

struct ETUIButton {
    virtual ~ETUIButton() = default;
    virtual void ET_onPress() = 0;
};

struct ETUIList {
    virtual ~ETUIList() = default;
    virtual Vec2i ET_getElemOffset(int idx) const = 0;
    virtual ListType ET_getListType() const = 0;
};

#endif /* __UI_ET_INTERFACES_HPP__ */