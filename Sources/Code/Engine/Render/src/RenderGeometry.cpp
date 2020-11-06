#include "RenderGeometry.hpp"
#include "Platform/OpenGL.hpp"
#include "Math/Matrix.hpp"

namespace {

int getVertexSize(VertexType vertType) {
    switch(vertType)
    {
    case VertexType::Vector3:
        return sizeof(Vec3);
    case VertexType::Vector4:
        return sizeof(Vec4);
    case VertexType::Particle:
        return sizeof(Vec4) + sizeof(Mat3x2);
    default:
        return 0;
    }
}

} // namespace

RenderGeometry::RenderGeometry() :
    aabb(Vec3(0.f), Vec3(0.f)),
    vaoId(0),
    vboId(0),
    extraVboId(0),
    vertCount(0),
    vertType(VertexType::Vector3) {
}

RenderGeometry::~RenderGeometry() {
}

void RenderGeometry::draw() {
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
    glBindVertexArray(0);
}

void RenderGeometry::drawChunk(const void* chunkBuffer, unsigned int chunkVertexCount) {
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getVertexSize(vertType) * chunkVertexCount, chunkBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, chunkVertexCount);
    glBindVertexArray(0);
}

void RenderGeometry::drawInstanced(const void* instacesData, unsigned int instancesCount) {
    glBindBuffer(GL_ARRAY_BUFFER, extraVboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getVertexSize(vertType) * instancesCount, instacesData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vaoId);
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertCount, instancesCount);
    glBindVertexArray(0);
}