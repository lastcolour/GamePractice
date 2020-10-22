#include "RenderGeometryManager.hpp"
#include "Core/ETLogger.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderGeometry.hpp"
#include "Math/Matrix.hpp"

namespace {

const int TEXT_CHUNK_VERTEX_COUNT = 6 * 32;
const int MAX_PARTICLES_COUNT = 256;

} // namespace

RenderGeometryManager::RenderGeometryManager() {
}

RenderGeometryManager::~RenderGeometryManager() {
}

bool RenderGeometryManager::init() {
    ETNode<ETRenderGeometryManager>::connect(getEntityId());
    ETNode<ETRenderResourceManager>::connect(getEntityId());
    return true;
}

void RenderGeometryManager::deinit() {
    ETNode<ETRenderResourceManager>::disconnect();
    ETNode<ETRenderGeometryManager>::disconnect();
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createGeometryOfType(PrimitiveGeometryType geomType) {
    switch(geomType) {
    case PrimitiveGeometryType::Square:
        return createSquare();
    case PrimitiveGeometryType::Sqaure_Tex:
        return createSquareTex();
    case PrimitiveGeometryType::Text_Vert_Chunk:
        return createTextVertexChunks();
    case PrimitiveGeometryType::Particles:
        return createParticles();
    default:
        break;
    }
    LogError("[RenderGeometryManager::createGeometryOfType] Can't create unknown type of geometry: '%d'", geomType);
    return nullptr;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::ET_createGeometry(PrimitiveGeometryType geomType) {
    auto reqGeomType = geomType;
    auto it = geometris.find(reqGeomType);
    if(it != geometris.end() && it->second) {
        return it->second;
    }
    auto geom = createGeometryOfType(reqGeomType);
    if(!geom) {
        return nullptr;
    }
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
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexVerts), squareTexVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB(Vec3(-1, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
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
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB(Vec3(-1, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = 6u;
    geometry->vertType = VertexType::Vector3;

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createTextVertexChunks() {
    GLuint vaoId = 0;
    GLuint vboId = 0;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * TEXT_CHUNK_VERTEX_COUNT, nullptr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB(0.f);
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = TEXT_CHUNK_VERTEX_COUNT;
    geometry->vertType = VertexType::Vector4;

    return geometry;
}

void RenderGeometryManager::ET_forgetResoruces() {
    geometris.clear();
}

void RenderGeometryManager::ET_cleanUnused() {
    auto it = geometris.begin();
    while(it != geometris.end()) {
        auto& geomPtr = it->second;
        if(geomPtr.use_count() == 1) {
            glDeleteBuffers(1, &(geomPtr->vboId));
            glDeleteVertexArrays(1, &(geomPtr->vaoId));
            it = geometris.erase(it);
        } else {
            ++it;
        }
    }
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createParticles() {
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
    GLuint extraVboId = 0;
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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    {
        glGenBuffers(1, &extraVboId);
        glBindBuffer(GL_ARRAY_BUFFER, extraVboId);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES_COUNT * 5 * sizeof(Vec4), nullptr, GL_STATIC_DRAW);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(Vec4), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(Vec4), reinterpret_cast<void*>(sizeof(Vec4)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(Vec4), reinterpret_cast<void*>(2 * sizeof(Vec4)));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(Vec4), reinterpret_cast<void*>(3 * sizeof(Vec4)));
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(Vec4), reinterpret_cast<void*>(4 * sizeof(Vec4)));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB(Vec3(-1, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->extraVboId = extraVboId;
    geometry->vertCount = 6u;
    geometry->vertType = VertexType::Particle;

    return geometry;
}