#include "RenderGeometryManager.hpp"
#include "RenderGeometry.hpp"
#include "Math/Matrix.hpp"
#include "Render/RenderCommon.hpp"
#include "RenderUtils.hpp"
#include "RenderConfig.hpp"

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

std::shared_ptr<RenderGeometry> RenderGeometryManager::createGeometryOfType(EPrimitiveGeometryType geomType) {
    switch(geomType) {
    case EPrimitiveGeometryType::Vec3:
        return createVec3();
    case EPrimitiveGeometryType::Vec3_Tex:
        return createVec3_Tex();
    case EPrimitiveGeometryType::Vec2_Tex:
        return createVec2_Tex();
    case EPrimitiveGeometryType::Particles:
        return createParticles();
    case EPrimitiveGeometryType::Line:
        return createLine();
    default:
        assert(false && "Invalid geometry type");
        break;
    }
    return nullptr;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::ET_createGeometry(EPrimitiveGeometryType geomType) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't get geometry without OpenGL context");

    auto geom = createGeometryOfType(geomType);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderFont::ET_createGeometry] Cant create geometry of type: %d (Error: %s)",
            geomType, errStr);
        return nullptr;
    }

    if(!geom) {
        return nullptr;
    }

    geom->geoType = geomType;

    geometries.push_back(geom);
    return geom;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createVec3() {
    static const GLfloat vertData[] = {
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

    const GLsizei vboStride = sizeof(Vec3);
    const GLsizei vboSize =  sizeof(vertData);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vboSize, vertData, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vboStride, static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->vaoId = vaoId;
    geometry->vbo = {vboId, static_cast<size_t>(vboSize)};

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createVec3_Tex() {
    static const GLfloat vertData[] = {
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

    const GLsizei vboStride = sizeof(Vec3) + sizeof(Vec2);
    const GLsizei vboSize = sizeof(vertData);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vboSize, vertData, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vboStride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vboStride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->vaoId = vaoId;
    geometry->vbo = {vboId, static_cast<size_t>(vboSize)};

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createVec2_Tex() {
    GLuint vaoId = 0;
    GLuint vboId = 0;

    const GLsizei vboStride = sizeof(Vec4);
    const GLsizei vboSize = 6 * sizeof(Vec4);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vboSize, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vboStride, static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->vaoId = vaoId;
    geometry->vbo = {vboId, static_cast<size_t>(vboSize)};

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createParticles() {
    static const GLfloat vertData[] = {
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

    const GLsizei vboStride = sizeof(Vec4);
    const GLsizei vboSize = sizeof(vertData);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vboSize, vertData, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vboStride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vboStride, reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    const auto maxParticles = Core::GetGlobal<RenderConfig>()->particlesConfig.maxParticles;
    const GLsizei extraVboStrite = sizeof(Vec4) + sizeof(Mat3x2);
    const GLsizei extraVboSize = extraVboStrite * maxParticles;

    {
        glGenBuffers(1, &extraVboId);
        glBindBuffer(GL_ARRAY_BUFFER, extraVboId);
        glBufferData(GL_ARRAY_BUFFER, extraVboSize, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, extraVboStrite, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, extraVboStrite, reinterpret_cast<void*>(sizeof(Vec4)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, extraVboStrite, reinterpret_cast<void*>(sizeof(Vec4) + sizeof(Vec2)));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, extraVboStrite, reinterpret_cast<void*>(sizeof(Vec4) + 2 * sizeof(Vec2)));
        glEnableVertexAttribArray(5);

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->vaoId = vaoId;
    geometry->vbo = {vboId, static_cast<size_t>(vboSize)};
    geometry->extraVbo =  {extraVboId, static_cast<size_t>(extraVboSize)};

    return geometry;
}

std::shared_ptr<RenderGeometry> RenderGeometryManager::createLine() {
    GLuint vaoId = 0;
    GLuint vboId = 0;

    const GLsizei vboStride = sizeof(Vec2) + sizeof(Vec4);
    const GLsizei vboSize = RenderUtils::MaxLinesPerDraw * vboStride;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    {
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vboSize, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vboStride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vboStride, reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->vaoId = vaoId;
    geometry->vbo = {vboId, static_cast<size_t>(vboSize)};

    return geometry;
}