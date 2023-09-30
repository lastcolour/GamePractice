#include "RenderGeometry.hpp"
#include "Platform/OpenGL.hpp"
#include "Math/Matrix.hpp"

#include <cassert>

RenderGeometry::RenderGeometry() :
    vaoId(0),
    geoType(EPrimitiveGeometryType::Vec3),
    bound(false) {
}

RenderGeometry::~RenderGeometry() {
}

void RenderGeometry::setVboData(const void* data, size_t bytes) {
    assert(!bound && "VAO is bounded");
    assert(bytes > 0 && "No data to update");
    assert(data && "Invalid data");

    if(vbo.size >= bytes) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {

        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LogDebug("[RenderGeometry::setVboData] Resizing vbo: %d -> %d", vbo.size, bytes);
        vbo.size = bytes;

        {
            bind();
            GLuint currVboId = 0;
            glGetVertexAttribIuiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,  &currVboId);
            unbind();

            if(currVboId != vbo.id) {
                LogError("[RenderGeometry::setVboData] VAO lost binding to VBO during resizing!");
            }
        }
    }
}

void RenderGeometry::setExtraVboData(const void* data, size_t bytes) {
    assert(!bound && "VAO is bounded");
    assert(bytes > 0 && "No data to update");
    assert(data && "Invalid data");

    if(extraVbo.size >= bytes) {
        glBindBuffer(GL_ARRAY_BUFFER, extraVbo.id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, extraVbo.id);
        glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LogDebug("[RenderGeometry::setExtraVboData] Resizing extra vbo: %d -> %d", extraVbo.size, bytes);
        extraVbo.size = bytes;
    }
}

void RenderGeometry::bind() {
    assert(!bound && "Geometry is already bound");

    bound = true;
    glBindVertexArray(vaoId);
}

void RenderGeometry::unbind() {
    assert(bound && "Geometry isn't bound");

    bound = false;
    glBindVertexArray(0);
}

bool RenderGeometry::isBound() const {
    return bound;
}

void RenderGeometry::drawTriangles(int start, int end) {
    assert(bound && "Geometry isn't bound");

    glDrawArrays(GL_TRIANGLES, start, end - start);
}

void RenderGeometry::drawTrianglesInstanced(int instances) {
    assert(bound && "Geometry isn't bound");

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances);
}

void RenderGeometry::drawLines(int start, int end) {
    assert(bound && "Geometry isn't bound");

    glDrawArrays(GL_LINES, start, end - start);
}