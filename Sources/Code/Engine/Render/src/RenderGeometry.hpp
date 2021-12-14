#ifndef __RENDER_GEOMETRY_HPP_
#define __RENDER_GEOMETRY_HPP_

#include "Math/AABB.hpp"

enum class VertexType {
    Vector3,
    Vector3_Tex,
    Vector4,
    Vector2_Tex,
    Particle,
    Line,
    Vector2
};

class RenderGeometry {
public:

    RenderGeometry();
    ~RenderGeometry();

    void bind();
    void unbind();

    void vboData(void* data, size_t bytes);
    void extraVboData(void* data, size_t bytes);

    void drawTriangles(int start, int end);
    void drawTrianglesInstanced(int start, int end, int instances);
    void drawLines(int start, int end);

    // void drawTriangles();
    // void drawText(const void* textData, unsigned int textVertCount);
    // void drawInstanced(const void* instaceData, unsigned int instancesCount);
    // void drawNinePatch(const Vec2& patch, const Vec2& uv);

public:

    // AABB2D aabb;
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int eboId;
    unsigned int extraVboId;
    // unsigned int vertCount;
    unsigned int indciesCount;
    VertexType vertType;
};

#endif /* __RENDER_GEOMETRY_HPP_ */