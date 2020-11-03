#ifndef __RENDER_AUX_FUNCTIONS_HPP__
#define __RENDER_AUX_FUNCTIONS_HPP__

#include "Math/Matrix.hpp"
#include "Core/Core.hpp"
#include "Render/Color.hpp"
#include "Math/Transform.hpp"

class RenderGeometry;

namespace Render {

const float TextNewLineOffset = 1.f;

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale, const RenderGeometry& geom);

Vec2 CalcGeomScaleForSize(const Vec2i& size, const RenderGeometry& geom);

} // namespace Render

#endif /* __RENDER_AUX_FUNCTIONS_HPP__ */