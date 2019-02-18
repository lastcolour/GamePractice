#include "Render/Render.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Environment.hpp"
#include "Surface.hpp"
#include "Logger.hpp"
#include "Assets.hpp"
#include "JSONNode.hpp"

#include <cassert>
#include <algorithm>

namespace {
    const GLsizei MAX_INFO_BUFF_SIZE = 255u;
    const char* MATERIALS = "Render/Materials.json";
    const char* SHADERS_ROOT_DIR = "Render/";
    const char* SQUARE_GEOM_NAME = "square";
}

Render::Render() :
    clearColor(0.f, 0.f, 0.f) {
}

bool Render::init() {
    auto surface = GetEnv()->getSurface();
    if(!surface) {
        assert(false && "Invalid surface");
        return false;
    }

    glViewport(0, 0, surface->getWidth(), surface->getHeight());
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    return true;
}

void Render::update() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    GetEnv()->getSurface()->swapBuffers();
}

void Render::setClearColor(const ColorF& col) {
    clearColor = col;
}

void Render::setViewport(int width, int heigth) {
    LogDebug("[Render::setViewport] Set viewport: [%ix%i]", width, heigth);
    glViewport(0, 0, width, heigth);
}

std::shared_ptr<RenderGeometry> Render::createGeometry(const std::string& geomName) {
    auto it = geometris.find(geomName);
    if(it != geometris.end() && !it->second.expired()) {
        return it->second.lock();
    }
    if(geomName == SQUARE_GEOM_NAME) {
        auto geom = createSquare();
        geometris[SQUARE_GEOM_NAME] = geom;
        return createSquare();
    } else {
        LogWarning("[Render::createGeometry] Can't create unknown type of geometry: '%s'", geomName);
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
    geometry->vaoId = vaoId;
    geometry->vboId = vboId;
    geometry->vertCount = sizeof(squareVerts) / vertElemSize;

    return geometry;
}

std::shared_ptr<RenderMaterial> Render::createMaterial(const std::string& matName) {
    if(matName.empty()) {
        LogError("[Render::createMaterial] Can't create material with empty name");
        return nullptr;
    }
    auto it = materials.find(matName);
    if(it != materials.end() && !it->second.expired()) {
        return it->second.lock();
    }
    auto assets = GetEnv()->getAssets();
    auto buffer = assets->loadAsset(MATERIALS);
    if(!buffer) {
        LogError("[Render::createMaterial] Can't load material from: %s", MATERIALS);
        return nullptr;
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[Render::createMaterial] Can't parse materials: %s", MATERIALS);
        return nullptr;
    }
    for(auto& matNode : rootNode) {
        if(matName != matNode.key()) {
            continue;
        }
        std::string vertShaderPath;
        std::string fragShaderPath;
        matNode.value("vert", vertShaderPath);
        matNode.value("frag", fragShaderPath);
        if(vertShaderPath.empty()) {
            LogError("[Render::createMaterial] Empty vert shader path in material: %s", matName);
            continue;
        }
        if(fragShaderPath.empty()) {
            LogError("[Render::createMaterial] Empty frag shader path in material: %s", matName);
            continue;
        }
        vertShaderPath = SHADERS_ROOT_DIR + vertShaderPath;
        fragShaderPath = SHADERS_ROOT_DIR + fragShaderPath;
        auto program = createProgram(vertShaderPath, fragShaderPath);
        if(!program) {
            LogError("[Render::createProgram] Can't create render material '%s' from vert: '%s' and frag: '%s' shaders",
                matName, vertShaderPath, fragShaderPath);
            return nullptr;
        }
        LogDebug("[Render::createMaterial] Load material: '%s'", matName);
        std::shared_ptr<RenderMaterial> material(new RenderMaterial(program));
        materials[matName] = material;
        return material;
    }
    return nullptr;
}

GLuint Render::createProgram(const std::string& vertFile, const std::string& fragFile) {
    auto assets = GetEnv()->getAssets();
    auto buffer = assets->loadAsset(vertFile);
    if(!buffer) {
        LogError("[Render::createProgram] Can't load vert shader file: %s", vertFile.c_str());
        return 0;
    }
    std::string vertSrc = buffer.getString();
    if(vertSrc.empty()) {
        LogError("[Render::createProgram] Loaded empty vert shader source from %s", vertFile.c_str());
        return 0;
    }
    buffer = assets->loadAsset(fragFile);
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

GLuint Render::createProgramImpl(const std::string& vertSrc, const std::string& fragSrc) {
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