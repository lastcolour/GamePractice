#include "Render/RenderGeometry.hpp"

RenderGeometry::RenderGeometry() {
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