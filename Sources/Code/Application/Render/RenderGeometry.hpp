#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Math/AABB.hpp"

enum class VertexType {
    Vec3,
    Vec4
};

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void draw();
    void drawChunk(const void* chunkBuffer, unsigned int chunkVertexCount);

public:

    AABB aabb;
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int vertCount;
    VertexType vertType;

private:

    int getVertSize() const;
};

#endif /* __RENDER_GEOMETRY_HPP_ */