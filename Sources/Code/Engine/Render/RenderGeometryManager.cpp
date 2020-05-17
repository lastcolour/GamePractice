#include "Render/RenderGeometryManager.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"
#include "Render/RenderGeometry.hpp"

namespace {

const char* SQUARE_GEOM_NAME = "square";
const char* SQUARE_TEX_GEOM_NAME = "square_tex";
const char* TEXT_VERTEX_CHUNK = "text_chunk";
const int TEXT_CHUNK_VERTEX_COUNT = 6 * 32;

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

std::shared_ptr<RenderGeometry> RenderGeometryManager::createGeometryOfType(const std::string& geomName) {
    if(geomName == SQUARE_GEOM_NAME) {
        return createSquare();
    } else if(geomName == SQUARE_TEX_GEOM_NAME) {
        return createSquareTex();
    } else if(geomName == TEXT_VERTEX_CHUNK) {
        return createTextVertexChunks();
    }
    LogWarning("[Render::createGeometryOfType] Can't create unknown type of geometry: '%s'", geomName);
    return nullptr;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::ET_createGeometry(const char* geomName) {
    std::string reqGeomName = geomName;
    auto it = geometris.find(reqGeomName);
    if(it != geometris.end() && it->second) {
        return it->second;
    }
    auto geom = createGeometryOfType(reqGeomName);
    if(!geom) {
        return nullptr;
    }
    geometris[reqGeomName] = geom;
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