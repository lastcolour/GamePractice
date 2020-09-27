#include "RenderCamera.hpp"
#include "Core/ETPrimitives.hpp"
#include "Platform/ETSurface.hpp"
#include "Core/ETLogger.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platform/OpenGL.hpp"

namespace {

const float DEF_Z_NEAR = -1.f;
const float DEF_Z_FAR = 1.f;

} // namespace

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

    LogDebug("[Render::updateRenderPort] Set viewport: [%ix%i]", newSize.x, newSize.y);
    viewport = newSize;

    glViewport(0, 0, viewport.x, viewport.y);

    Math::ProjectOrtho(projection, 0.f, static_cast<float>(viewport.x),
        0.f, static_cast<float>(viewport.y), DEF_Z_NEAR, DEF_Z_FAR);

    ET_QueueEvent(&ETRenderCameraEvents::ET_onRenderPortResized, newSize);
}

const Vec2i& RenderCamera::ET_getRenderPort() const {
    return viewport;
}

const Mat4& RenderCamera::ET_getProj2DMat4() const {
    return projection;
}