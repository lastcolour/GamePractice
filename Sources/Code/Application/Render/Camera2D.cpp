#include "Render/Camera2D.hpp"
#include "Math/MatrixTransform.hpp"

namespace {
    const float DEF_Z_NEAR = -1.f;
    const float DEF_Z_FAR = 1.f;
} // namespace

Camera2D::Camera2D() :
    projection(1.f),
    viewport(0, 0) {
}

Camera2D::~Camera2D() {
}

void Camera2D::setViewport(int w, int h) {
    viewport = Vec2i(w, h);
    Math::ProjectOrtho(projection, 0.f, static_cast<float>(viewport.x),
        0.f, static_cast<float>(viewport.y), DEF_Z_NEAR, DEF_Z_FAR);
}

const Vec2i& Camera2D::getViewport() const {
    return viewport;
}

const Mat4& Camera2D::getProjMat4() const {
    return projection;
}