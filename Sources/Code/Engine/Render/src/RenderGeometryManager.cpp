#include "RenderGeometryManager.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderGeometry.hpp"
#include "Math/Matrix.hpp"
#include "Render/RenderCommon.hpp"
#include "RenderUtils.hpp"

#include <cassert>

RenderGeometryManager::RenderGeometryManager() {
}

RenderGeometryManager::~RenderGeometryManager() {
}

bool RenderGeometryManager::init() {
    ETNode<ETRenderGeometryManager>::connect(getEntityId());
    return true;
}

void RenderGeometryManager::deinit() {
    ETNode<ETRenderGeometryManager>::disconnect();
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createGeometryOfType(PrimitiveGeometryType geomType) {
    switch(geomType) {
    case PrimitiveGeometryType::Square:
        return createSquare();
    case PrimitiveGeometryType::Sqaure_Tex:
        return createSquareTex();
    case PrimitiveGeometryType::Text:
        return createText();
    case PrimitiveGeometryType::Particles:
        return createParticles();
    case PrimitiveGeometryType::NinePatch:
        return createNinePatch();
    case PrimitiveGeometryType::Line:
        return createLine();
    default:
        assert(false && "Invalid geometry type");
        break;
    }
    return nullptr;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::ET_createGeometry(PrimitiveGeometryType geomType) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't get geometry without OpenGL context");

    auto reqGeomType = geomType;
    auto it = geometris.find(reqGeomType);
    if(it != geometris.end() && it->second) {
        return it->second;
    }

    auto geom = createGeometryOfType(reqGeomType);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderFont::ET_createGeometry] Cant create geometry of type: %d (Error: %s)",
            geomType, errStr);
        return nullptr;
    }

    if(!geom) {
        return nullptr;
    }

    assert(geom->aabb.getCenter() == Vec2(0.f) && "Invalid geometry center");

    geometris[reqGeomType] = geom;
    return geom;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createSquareTex() {
    static const GLfloat squareTexVerts[] = {
        // first tri
        -1.f, -1.f, 0.f,    0.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        // second tri
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
         1.f, -1.f, 0.f,    1.f, 0.f
    };

    GLuint vaoId = 0;
    GLuint vboId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexVerts), squareTexVerts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB2D(Vec2(-1.f, -1.f), Vec2(1.f, 1.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = 6u;
    geometry->vertType = VertexType::Vector3_Tex;

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createSquare() {
    static const GLfloat squareVerts[] = {
        // first tri
        -1.f, -1.f, 0.f,
        -1.f,  1.f, 0.f,
         1.f, -1.f, 0.f,
        // second tri
        -1.f,  1.f, 0.f,
         1.f,  1.f, 0.f,
         1.f, -1.f, 0.f,
    };

    GLuint vaoId = 0;
    GLuint vboId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB2D(Vec2(-1.f, -1.f), Vec2(1.f, 1.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = 6u;
    geometry->vertType = VertexType::Vector3;

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createText() {
    GLuint vaoId = 0;
    GLuint vboId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vec4) * RenderUtils::MaxCharsPerDraw, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB2D(0.f);
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = RenderUtils::MaxCharsPerDraw;
    geometry->vertType = VertexType::Vector4;

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createParticles() {
    static const GLfloat squareTexVerts[] = {
        // first tri
        -1.f, -1.f,    0.f, 0.f,
        -1.f,  1.f,    0.f, 1.f,
         1.f, -1.f,    1.f, 0.f,
        // second tri
        -1.f,  1.f,    0.f, 1.f,
         1.f,  1.f,    1.f, 1.f,
         1.f, -1.f,    1.f, 0.f
    };

    GLuint vaoId = 0;
    GLuint vboId = 0;
    GLuint extraVboId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexVerts), squareTexVerts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    {
        GLsizei stride = sizeof(Vec4) + sizeof(Mat3x2);

        glGenBuffers(1, &extraVboId);
        glBindBuffer(GL_ARRAY_BUFFER, extraVboId);
        glBufferData(GL_ARRAY_BUFFER, RenderUtils::MaxParticlessPerDraw * stride, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(Vec4)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(Vec4) + sizeof(Vec2)));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(Vec4) + 2 * sizeof(Vec2)));
        glEnableVertexAttribArray(5);

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB2D(Vec2(-1.f, -1.f), Vec2(1.f, 1.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->extraVboId = extraVboId;
    geometry->vertCount = 6u;
    geometry->vertType = VertexType::Particle;

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createNinePatch() {
    const unsigned int indciesCount = 2 * 3 * 9;
    GLushort indicies[indciesCount];
    int offset = 0;
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            GLushort k = static_cast<GLushort>(4 * i + j);

            indicies[offset] = k;
            indicies[offset + 1] = k + 4;
            indicies[offset + 2] = k + 1;

            indicies[offset + 3] = k + 4;
            indicies[offset + 4] = k + 5;
            indicies[offset + 5] = k + 1;

            offset += 6;
        }
    }

    GLuint vaoId = 0;
    GLuint vboId = 0;
    GLuint eboId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &eboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indciesCount * sizeof(GLushort), &indicies[0], GL_STATIC_DRAW);
    }
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 16, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB2D(Vec2(-1.f, -1.f), Vec2(1.f, 1.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->eboId = eboId;
    geometry->vertCount = 16;
    geometry->indciesCount = indciesCount;
    geometry->vertType = VertexType::Vector2_Tex;

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createLine() {
    GLuint vaoId = 0;
    GLuint vboId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        GLsizei stride = sizeof(Vec2) + sizeof(Vec4);

        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, RenderUtils::MaxLinesPerDraw * stride, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB2D(0.f);
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = 2u;
    geometry->vertType = VertexType::Line;

    return geometry;
}