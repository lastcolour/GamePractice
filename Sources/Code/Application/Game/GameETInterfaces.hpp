#ifndef __LOGICS_ET_INTERFACES_HPP__
#define __LOGICS_ET_INTERFACES_HPP__

#include "Core/ETPrimitives.hpp"
#include "Math/Vector.hpp"
#include "Render/Color.hpp"

#include <string>

struct RenderLogicParams {
    Vec2 pt;
    Vec2 size;
    ColorF col;
    float rot;

    RenderLogicParams() :
        pt(0.f),
        size(0.f),
        col(1.f, 1.f, 1.f),
        rot(0.f) {}
};

struct ETRenderLogic {
    virtual ~ETRenderLogic() = default;
    virtual void ET_setRenderParams(const RenderLogicParams& params) = 0;
    virtual void ET_getRenderParams(RenderLogicParams& params) = 0;
};

struct ETGameObject {
    virtual ~ETGameObject() = default;
    virtual const std::string& ET_getName() const = 0;
};

struct ETGame {
    virtual EntityId ET_createGameObject(const std::string& objectName) = 0;
    virtual void ET_destroyObject(EntityId entId) = 0;
};

#endif /* __LOGICS_ET_INTERFACES_HPP__ */