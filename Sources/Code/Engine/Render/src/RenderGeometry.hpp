#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Math/AABB.hpp"
#include "Render/RenderCommon.hpp"

struct RenderBufferInfo {
    unsigned int id {0};
    size_t size {0};
};

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void bind();
    void unbind();
    bool isBound() const;

    void setVboData(const void* data, size_t bytes);
    void setExtraVboData(const void* data, size_t bytes);

    void drawTriangles(int start, int end);
    void drawTrianglesInstanced(int instances);
    void drawLines(int start, int end);

private:

    RenderGeometry(const RenderGeometry&) = delete;
    RenderGeometry& operator=(const RenderGeometry&) = delete;

public:

    unsigned int vaoId;
    RenderBufferInfo vbo;
    RenderBufferInfo ebo;
    RenderBufferInfo extraVbo;
    EPrimitiveGeometryType geoType;
    bool bound;
};

#endif /* __RENDER_GEOMETRY_HPP_ */