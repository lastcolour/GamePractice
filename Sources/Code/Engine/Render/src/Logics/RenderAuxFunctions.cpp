#include "Logics/RenderAuxFunctions.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderInterfaces.hpp"

namespace Render {

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale, const RenderGeometry& geom) {
    Mat4 resMat(1.f);
    const Vec3 center = geom.aabb.getCenter();
    Math::Translate(resMat, center);
    Math::Rotate(resMat, tm.quat);
    Math::Translate(resMat, tm.pt);
    Vec3 resScale = Vec3(scale.x * tm.scale.x, scale.y * tm.scale.y, scale.z * tm.scale.z);
    Math::Scale(resMat, resScale);
    return resMat;
}

Vec2 CalcGeomScaleForSize(const Vec2i& size, const RenderGeometry& geom) {
    const auto& aabbSize = geom.aabb.getSize();
    Vec2 scale = Vec2(static_cast<float>(size.x), static_cast<float>(size.y))
        / Vec2(aabbSize.x, aabbSize.y);
    return scale;
}

Vec2i CaclScaledGeomSize(const Vec2& scale, const RenderGeometry& geom) {
    const auto& aabbSize = geom.aabb.getSize();
    Vec2i size = Vec2i(static_cast<int>(aabbSize.x * scale.x),
        static_cast<int>(aabbSize.y * scale.y));
    return size;
}

bool IsRenderThread() {
    bool res = false;
    ET_SendEventReturn(res, &ETRender::ET_isRenderThread);
    return res;
}

} // namespace Render