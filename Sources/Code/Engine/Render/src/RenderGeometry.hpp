#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Math/AABB.hpp"

enum class VertexType {
    Vector3,
    Vector3_Tex,
    Vector4,
    Particle
};

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void draw();
    void drawChunk(const void* chunkBuffer, unsigned int chunkVertexCount);
    void drawInstanced(const void* instaceData, unsigned int instancesCount);

public:

    AABB aabb;
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int extraVboId;
    unsigned int vertCount;
    VertexType vertType;

private:

    int getVertSize() const;
};

#endif /* __RENDER_GEOMETRY_HPP_ */