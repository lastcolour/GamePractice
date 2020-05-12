#ifndef __RENDER_AUX_FUNCTIONS_HPP__
#define __RENDER_AUX_FUNCTIONS_HPP__

#include "Math/Matrix.hpp"
#include "Core/Core.hpp"
#include "Render/Color.hpp"

class RenderGeometry;
class JSONNode;

namespace Render {

Mat4 CalcModelMat(EntityId entityId, const Vec3& scale, const RenderGeometry& geom);

Vec2 CalcGeomScaleForSize(const Vec2i& size, const RenderGeometry& geom);

Vec2i CaclScaledGeomSize(const Vec2& scale, const RenderGeometry& geom);

} // namespace Render

#endif /* __RENDER_AUX_FUNCTIONS_HPP__ */