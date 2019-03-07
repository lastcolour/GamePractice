#include "Render/Render.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Render/RenderTextureFramebuffer.hpp"

#include "Platforms/OpenGL.hpp"

#include <cassert>
#include <algorithm>
#include <type_traits>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

namespace {
    const int MAX_INFO_BUFF_SIZE = 255u;
    const char* MATERIALS = "Render/Materials.json";
    const char* SHADERS_ROOT_DIR = "Render/";
    const char* SQUARE_GEOM_NAME = "square";
}

Render::Render() :
    clearColor(0.f, 0.f, 0.f),
    renderFb(nullptr) {
}

bool Render::onInit() {
    GLContextType glCtxType = GLContextType::None;
    ET_SendEventReturn(glCtxType, &ETSurface::ET_getGLContextType);
    if(glCtxType == GLContextType::None) {
        LogError("[Render::onInit] Can't init render without GL context");
        return false;
    }

    ETNode<ETRender>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());

    Vec2i size;
    ET_SendEventReturn(size, &ETSurface::ET_getSize);
    setViewport(size);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    return true;
}

void Render::onUpdate() {
    ET_drawFrame();
}

void Render::ET_drawFrame() {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible && !renderFb) {
        return;
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderContext renderCtx;
    renderCtx.proj2dMat = camera2d.getProjMat4();

    ET_SendEvent(&ETRenderEvents::ET_onRender, renderCtx);

    if(!renderFb) {
        ET_SendEvent(&ETSurface::ET_swapBuffers);
    }
}

const Mat4& Render::ET_getProj2DMat4() const {
    return camera2d.getProjMat4();
}

const ColorF& Render::ET_getClearColor() const {
    return clearColor;
}

void Render::ET_setClearColor(const ColorF& col) {
    clearColor = col;
}

Vec2i Render::ET_getRenderPort() const {
    return camera2d.getViewport();
}

void Render::ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFramebuffer) {
    if(renderFb == renderFramebuffer) {
        return;
    } else if(renderFramebuffer == nullptr) {
        Vec2i size;
        ET_SendEventReturn(size, &ETSurface::ET_getSize);
        setViewport(size);
        renderFb = nullptr;
    } else {
        renderFb = renderFramebuffer;
        setViewport(renderFramebuffer->getSize());
    }
}

std::shared_ptr<RenderGeometry> Render::ET_createGeometry(const std::string& geomName) {
    std::string reqGeomName = geomName;
    std::transform(reqGeomName.begin(), reqGeomName.end(), reqGeomName.begin(), tolower);
    auto it = geometris.find(reqGeomName);
    if(it != geometris.end() && !it->second.expired()) {
        return it->second.lock();
    }
    if(reqGeomName == SQUARE_GEOM_NAME) {
        auto geom = createSquare();
        geometris[reqGeomName] = geom;
        return geom;
    } else {
        LogWarning("[Render::createGeometry] Can't create unknown type of geometry: '%s'", reqGeomName);
        return nullptr;
    }
}

std::shared_ptr<RenderGeometry> Render::createSquare() {
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

    const size_t vertElemSize = 3u * sizeof(GLfloat);

    GLuint vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    GLuint vboId;
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertElemSize, static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    std::shared_ptr<RenderGeometry> geometry(new RenderGeometry);
    geometry->aabb = AABB(Vec3(-1, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = sizeof(squareVerts) / vertElemSize;

    return geometry;
}

std::shared_ptr<RenderMaterial> Render::ET_createMaterial(const std::string& matName) {
    std::string reqMatName = matName;
    std::transform(reqMatName.begin(), reqMatName.end(), reqMatName.begin(), tolower);
    if(reqMatName.empty()) {
        LogError("[Render::createMaterial] Can't create material with empty name");
        return nullptr;
    }
    auto it = materials.find(reqMatName);
    if(it != materials.end() && !it->second.expired()) {
        return it->second.lock();
    }
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAsset::ET_loadJSONAsset, MATERIALS);
    if(!rootNode) {
        LogError("[Render::createMaterial] Can't create materials '%s' from: %s", matName, MATERIALS);
        return nullptr;
    }
    for(auto& matNode : rootNode) {
        if(reqMatName != matNode.key()) {
            continue;
        }
        std::string vertShaderPath;
        std::string fragShaderPath;
        matNode.value("vert", vertShaderPath);
        matNode.value("frag", fragShaderPath);
        if(vertShaderPath.empty()) {
            LogError("[Render::createMaterial] Empty vert shader path in material: %s", reqMatName);
            continue;
        }
        if(fragShaderPath.empty()) {
            LogError("[Render::createMaterial] Empty frag shader path in material: %s", reqMatName);
            continue;
        }
        vertShaderPath = SHADERS_ROOT_DIR + vertShaderPath;
        fragShaderPath = SHADERS_ROOT_DIR + fragShaderPath;
        auto program = createProgram(vertShaderPath, fragShaderPath);
        if(!program) {
            LogError("[Render::createProgram] Can't create render material '%s' from vert: '%s' and frag: '%s' shaders",
                reqMatName, vertShaderPath, fragShaderPath);
            return nullptr;
        }
        LogDebug("[Render::createMaterial] Load material: '%s'", reqMatName);
        std::shared_ptr<RenderMaterial> material(new RenderMaterial(program));
        materials[reqMatName] = material;
        return material;
    }
    return nullptr;
}

int Render::createProgram(const std::string& vertFile, const std::string& fragFile) {
    Buffer buffer;
    ET_SendEventReturn(buffer, &ETAsset::ET_loadAsset, vertFile);
    if(!buffer) {
        LogError("[Render::createProgram] Can't load vert shader file: %s", vertFile.c_str());
        return 0;
    }
    std::string vertSrc = buffer.getString();
    if(vertSrc.empty()) {
        LogError("[Render::createProgram] Loaded empty vert shader source from %s", vertFile.c_str());
        return 0;
    }
    ET_SendEventReturn(buffer, &ETAsset::ET_loadAsset, fragFile);
    if(!buffer) {
        LogError("[Render::createProgram] Can't load frag shader file: %s", fragFile.c_str());
        return 0;
    }
    std::string fragSrc = buffer.getString();
    if(fragSrc.empty()) {
        LogError("[Render::createProgram] Loaded empty fragFile shader source from %s", vertFile.c_str());
        return 0;
    }
    return createProgramImpl(vertSrc, fragSrc);
}

int Render::createProgramImpl(const std::string& vertSrc, const std::string& fragSrc) {
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    if(!vertShaderId) {
        LogWarning("[Render::createProgramImpl] Can't create vertext shader");
        return 0;
    }
    const char* cSrcStr = vertSrc.c_str();
    glShaderSource(vertShaderId, 1, &cSrcStr, nullptr);
    glCompileShader(vertShaderId);
    GLint status = GL_FALSE;
    char infoBuffer[MAX_INFO_BUFF_SIZE];
    glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        glGetShaderInfoLog(vertShaderId, MAX_INFO_BUFF_SIZE, nullptr, infoBuffer);
        LogWarning("[Render::createProgramImpl] Can't compile vertex shader: %s", infoBuffer);
        glDeleteShader(vertShaderId);
        return 0;
    }

    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    if(!fragShaderId) {
        LogWarning("[Render::createProgramImpl] Can't create fragment shader");
        glDeleteShader(vertShaderId);
        glDeleteShader(fragShaderId);
        return 0;
    }
    cSrcStr = fragSrc.c_str();
    glShaderSource(fragShaderId, 1, &cSrcStr, nullptr);
    glCompileShader(fragShaderId);
    glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        glGetShaderInfoLog(fragShaderId, MAX_INFO_BUFF_SIZE, nullptr, infoBuffer);
        glDeleteShader(vertShaderId);
        glDeleteShader(fragShaderId);
        LogWarning("[Render::createProgramImpl] Can't compile fragment shader: %s", infoBuffer);
        return 0;
    }

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    if(status != GL_TRUE) {
        glGetProgramInfoLog(programId, MAX_INFO_BUFF_SIZE, nullptr, infoBuffer);
        glDeleteShader(vertShaderId);
        glDeleteShader(fragShaderId);
        glDeleteProgram(programId);
        LogWarning("[Render::createProgramImpl] Can't link render program: %s", infoBuffer);
        return 0;
    }
    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
    return programId;
}

void Render::ET_onSurfaceResize(const Vec2i& size) {
    if(!renderFb) {
        setViewport(size);
    }
}

void Render::setViewport(const Vec2i& viewport) {
    LogDebug("[Render::setViewport] Set viewport: [%ix%i]", viewport.x, viewport.y);
    camera2d.setViewport(viewport.x, viewport.y);
    glViewport(0, 0, viewport.x, viewport.y);
}