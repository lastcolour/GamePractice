#ifndef __UI_ET_INTERFACES_HPP__
#define __UI_ET_INTERFACES_HPP__

#include "Math/AABB.hpp"

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual const AABB2Di& ET_getAaabb2di() const = 0;
};

struct ETUIButton {
    virtual ~ETUIButton() = default;
};

struct ETUIList {
    virtual ~ETUIList() = default;
};

#endif /* __UI_ET_INTERFACES_HPP__ */