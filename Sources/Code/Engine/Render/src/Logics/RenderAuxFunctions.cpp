#include "Logics/RenderAuxFunctions.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderInterfaces.hpp"

namespace Render {

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale, const RenderGeometry& geom) {
    Mat4 resMat(1.f);
    const Vec3 center = geom.aabb.getCenter();
    Math::AddTranslate(resMat, tm.pt);
    Math::AddRotate(resMat, tm.quat);
    Math::AddTranslate(resMat, center);
    Math::AddScale(resMat, tm.scale.getScaled(scale));
    return resMat;
}

Vec2 CalcGeomScaleForSize(const Vec2& size, const RenderGeometry& geom) {
    const auto& aabbSize = geom.aabb.getSize();
    Vec2 scale = Vec2(size.x, size.y) / Vec2(aabbSize.x, aabbSize.y);
    return scale;
}

} // namespace Render