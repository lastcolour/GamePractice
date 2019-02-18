#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Platforms/OpenGL.hpp"

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void draw();

public:

    GLuint vaoId;
    GLuint vboId;
    size_t vertCount;
};

#endif /* __RENDER_GEOMETRY_HPP_ */