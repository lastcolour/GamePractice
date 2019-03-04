#include "Render/Camera2D.hpp"
#include "Math/Transform.hpp"

namespace {
    const float DEF_Z_NEAR = -1.f;
    const float DEF_Z_FAR = 1.f;
}

Camera2D::Camera2D() :
    projection(1.f),
    viewport(0.f, 0.f) {
}

Camera2D::~Camera2D() {
}

void Camera2D::setViewport(int w, int h) {
    viewport = Vec2(static_cast<float>(w), static_cast<float>(h));
    Math::ProjectOrtho(projection, 0.f, viewport.x,
        0.f, viewport.y, DEF_Z_NEAR, DEF_Z_FAR);
}

const Vec2& Camera2D::getViewport() const {
    return viewport;
}

const Mat4& Camera2D::getProjMat4() const {
    return projection;
}