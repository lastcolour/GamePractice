#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Math/AABB.hpp"

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void draw();

public:

    AABB aabb;
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int vertCount;
};

#endif /* __RENDER_GEOMETRY_HPP_ */