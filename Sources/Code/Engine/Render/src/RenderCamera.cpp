#include "RenderCamera.hpp"
#include "Platform/ETSurface.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderConfig.hpp"
#include "Core/GlobalData.hpp"

RenderCamera::RenderCamera() :
    projection(1.f),
    viewport(0, 0) {
}

RenderCamera::~RenderCamera() {
}

bool RenderCamera::init() {
    ETNode<ETRenderCamera>::connect(getEntityId());

    Vec2i size(0);
    ET_SendEventReturn(size, &ETSurface::ET_getSize);
    ET_setRenderPort(size);

    return true;
}

void RenderCamera::deinit() {
    ETNode<ETRenderCamera>::disconnect();
}

void RenderCamera::ET_setRenderPort(const Vec2i& newSize) {
    if(newSize == viewport) {
        return;
    }

    viewport = newSize;

    glViewport(0, 0, viewport.x, viewport.y);

    auto& cameraConfig = GetGlobal<RenderConfig>()->cameraConfig;
    auto zNear = cameraConfig.zNear;
    auto zFar = std::max(zNear + 0.001f, cameraConfig.zFar);

    Math::ProjectOrtho(projection, 0.f, static_cast<float>(viewport.x),
        0.f, static_cast<float>(viewport.y), zNear, zFar);

    ET_QueueEvent(&ETRenderCameraEvents::ET_onRenderPortResized, newSize);
}

const Vec2i& RenderCamera::ET_getRenderPort() const {
    return viewport;
}

const Mat4& RenderCamera::ET_getProj2DMat4() const {
    return projection;
}