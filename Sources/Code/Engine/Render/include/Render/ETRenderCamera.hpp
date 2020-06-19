#ifndef __ET_RENDER_CAMERA_HPP__
#define __ET_RENDER_CAMERA_HPP__

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

struct ETRenderCameraEvents {
    virtual ~ETRenderCameraEvents() = default;
    virtual void ET_onRenderPortResized() = 0;
};

struct ETRenderCamera {
    virtual ~ETRenderCamera() = default;
    virtual const Mat4& ET_getProj2DMat4() const = 0;
    virtual void ET_setRenderPort(const Vec2i& newViewPort) = 0;
    virtual const Vec2i& ET_getRenderPort() const = 0;
};

#endif /* __ET_RENDER_CAMERA_HPP__ */