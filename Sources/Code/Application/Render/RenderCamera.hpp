#ifndef __RENDER_CAMERA_HPP__
#define __RENDER_CAMERA_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class RenderCamera : public SystemLogic,
    ETNode<ETRenderCamera> {
public: 

    RenderCamera();
    virtual ~RenderCamera();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderCamera
    const Mat4& ET_getProj2DMat4() const override;
    void ET_setRenderPort(const Vec2i& newViewPort) override;
    const Vec2i& ET_getRenderPort() const override;

private:

    Mat4 projection;
    Vec2i viewport;
};

#endif /* __RENDER_CAMERA_HPP__ */