#ifndef __RENDER_AUX_FUNCTIONS_HPP__
#define __RENDER_AUX_FUNCTIONS_HPP__

#include "Math/Matrix.hpp"
#include "Core/Core.hpp"
#include "Render/Color.hpp"
#include "Math/Transform.hpp"

class RenderGeometry;

namespace Render {

const float TextNewLineOffset = 1.f;

const int MaxParticlessPerDraw = 1024;

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale);

} // namespace Render

#endif /* __RENDER_AUX_FUNCTIONS_HPP__ */