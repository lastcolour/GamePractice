#include "Render/RenderGeometry.hpp"
#include "Platforms/OpenGL.hpp"

RenderGeometry::RenderGeometry() :
    aabb(Vec3(0.f), Vec3(0.f)),
    vaoId(0),
    vboId(0),
    vertCount(0) {
}

RenderGeometry::~RenderGeometry() {
    glDeleteBuffers(1, &vboId);
    glDeleteVertexArrays(1, &vaoId);
}

void RenderGeometry::draw() {
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
    glBindVertexArray(0);
}