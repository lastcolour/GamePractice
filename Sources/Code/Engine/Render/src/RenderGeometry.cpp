#include "RenderGeometry.hpp"
#include "Platform/OpenGL.hpp"
#include "Math/Matrix.hpp"

#include <cassert>

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
    case VertexType::Vector2_Tex:
        return sizeof(Vec4);
    case VertexType::Vector2:
        return sizeof(Vec2);
    default:
        assert(false && "Invalid vertex type");
        return 0;
    }
}

} // namespace

RenderGeometry::RenderGeometry() :
    aabb(0.f),
    vaoId(0),
    vboId(0),
    eboId(0),
    extraVboId(0),
    vertCount(0),
    indciesCount(0),
    vertType(VertexType::Vector3) {
}

RenderGeometry::~RenderGeometry() {
}

void RenderGeometry::drawTriangles() {
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
    glBindVertexArray(0);
}

void RenderGeometry::drawLine(const void* vertexData) {
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getVertexSize(vertType) * vertCount, vertexData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_LINES, 0, vertCount);
    glBindVertexArray(0);
}

void RenderGeometry::drawText(const void* textData, unsigned int textVertCount) {
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getVertexSize(vertType) * textVertCount, textData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, textVertCount);
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

void RenderGeometry::drawNinePatch(const Vec2& patchPt, const Vec2& patchUV) {
    float xLeft = Math::Lerp(-1.f, 0.f, patchPt.x);
    float xRight = Math::Lerp(0.f, 1.f, 1.f - patchPt.x);
    float yTop = Math::Lerp(0.f, 1.f, 1.f - patchPt.y);
    float yBot = Math::Lerp(-1.f, 0.f, patchPt.y);

    float uLeft = patchUV.x;
    float uRight = 1.f - patchUV.x;
    float vTop = 1.f - patchUV.y;
    float vBot = patchUV.y;

    Vec4 verticies[] = {
        Vec4(-1.f, -1.f, 0.f, 0.f),  Vec4(xLeft, -1.f, uLeft, 0.f),  Vec4(xRight, -1.f, uRight, 0.f),  Vec4(1.f, -1.f, 1.f, 0.f),
        Vec4(-1.f, yBot, 0.f, vBot), Vec4(xLeft, yBot, uLeft, vBot), Vec4(xRight, yBot, uRight, vBot), Vec4(1.f, yBot, 1.f, vBot),
        Vec4(-1.f, yTop, 0.f, vTop), Vec4(xLeft, yTop, uLeft, vTop), Vec4(xRight, yTop, uRight, vTop), Vec4(1.f, yTop, 1.f, vTop),
        Vec4(-1.f, 1.f,  0.f, 1.f),  Vec4(xLeft, 1.f,  uLeft, 1.f),  Vec4(xRight, 1.f,  uRight, 1.f),  Vec4(1.f, 1.f,  1.f, 1.f),
    };

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * 16, verticies[0].getPtr());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, indciesCount, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}