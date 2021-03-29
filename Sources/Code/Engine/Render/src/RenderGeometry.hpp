#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Math/AABB.hpp"

enum class VertexType {
    Vector3,
    Vector3_Tex,
    Vector4,
    Vector2_Tex,
    Particle,
    Vector2
};

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void drawTriangles();
    void drawChunk(const void* chunkBuffer, unsigned int chunkVertexCount);
    void drawInstanced(const void* instaceData, unsigned int instancesCount);
    void drawNinePatch(const Vec2& patch, const Vec2& uv);
    void drawLine(const void* vertexData);

public:

    AABB2D aabb;
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int eboId;
    unsigned int extraVboId;
    unsigned int vertCount;
    unsigned int indciesCount;
    VertexType vertType;
};

#endif /* __RENDER_GEOMETRY_HPP_ */