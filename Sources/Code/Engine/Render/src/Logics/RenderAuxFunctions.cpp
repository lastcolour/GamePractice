#include "Logics/RenderAuxFunctions.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderInterfaces.hpp"

namespace Render {

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale) {
    Mat4 resMat(1.f);
    Math::AddTranslate(resMat, tm.pt);
    Math::AddRotate(resMat, tm.quat);
    Math::AddScale(resMat, tm.scale.getScaled(scale / 2.f));
    return resMat;
}

} // namespace Render