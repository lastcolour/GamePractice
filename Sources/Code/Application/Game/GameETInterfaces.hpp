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
};

class ETRenderLogic {
public:

    virtual ~ETRenderLogic() = default;

    virtual void ET_setRenderParams(const RenderLogicParams& params) = 0;
};

class ETTouchLogic {
public:

    virtual ~ETTouchLogic() = default;

    virtual void ET_onTouch(int x, int y) {}
};

class ETGameObject {
public:

    virtual ~ETGameObject() = default;

    virtual const std::string& ET_getName() const = 0;
};

class ETGame {
public:

    virtual EntityId ET_createGameObject(const std::string& objectName) = 0;
};

#endif /* __LOGICS_ET_INTERFACES_HPP__ */