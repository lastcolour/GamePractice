#include "Logics/RenderAuxFunctions.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderInterfaces.hpp"

namespace Render {

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale) {
    auto resMat = tm.toMat4();
    Math::AddScale(resMat, scale / 2.f);
    return resMat;
}

} // namespace Render