#include "RenderShaderManager.hpp"
#include "Core/ETAssets.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderShader.hpp"
#include "RenderUtils.hpp"
#include "RenderConfig.hpp"

#include <cassert>

namespace {

const int MAX_INFO_BUFF_SIZE = 512u;
const int INVALID_PROGRAM_ID = 0;

} // namespace

RenderShaderManager::RenderShaderManager() {
}

RenderShaderManager::~RenderShaderManager() {
}

bool RenderShaderManager::init() {
    ETNode<ETRenderShaderManager>::connect(getEntityId());
    return true;
}

void RenderShaderManager::deinit() {
    ETNode<ETRenderShaderManager>::disconnect();
}

std::shared_ptr<RenderShader> RenderShaderManager::ET_createShader(const char* shaderName) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't create shader without OpenGL context");

    std::string reqShaderName = shaderName;
    if(reqShaderName.empty()) {
        LogWarning("[RenderShaderManager::createShader] Can't create shader with empty name");
        return nullptr;
    }
    auto it = shaders.find(reqShaderName);
    if(it != shaders.end() && it->second) {
        return it->second;
    }
    auto programId = createShaderProgram(reqShaderName);
    if(!programId) {
        return nullptr;
    }
    std::shared_ptr<RenderShader> shader(new RenderShader(programId));
    shader->bind();
    shader->cacheUniformIds();
    shader->unbind();
    shaders[reqShaderName] = shader;
    return shader;
}

int RenderShaderManager::createProgram(const std::string& vertFile, const std::string& fragFile) {
    Memory::Buffer buffer;
    ET_SendEventReturn(buffer, &ETAssets::ET_loadAsset, vertFile.c_str());
    if(!buffer) {
        LogWarning("[RenderShaderManager::createProgram] Can't load vert shader file: %s", vertFile.c_str());
        return 0;
    }
    std::string vertSrc = buffer.acquireString();
    if(vertSrc.empty()) {
        LogWarning("[RenderShaderManager::createProgram] Loaded empty vert shader source from %s", vertFile.c_str());
        return 0;
    }
    ET_SendEventReturn(buffer, &ETAssets::ET_loadAsset, fragFile.c_str());
    if(!buffer) {
        LogWarning("[RenderShaderManager::createProgram] Can't load frag shader file: %s", fragFile.c_str());
        return 0;
    }
    std::string fragSrc = buffer.acquireString();
    if(fragSrc.empty()) {
        LogWarning("[RenderShaderManager::createProgram] Loaded empty fragFile shader source from %s", vertFile.c_str());
        return 0;
    }
    return createProgramImpl(vertSrc, fragSrc);
}

int RenderShaderManager::createProgramImpl(const std::string& vertSrc, const std::string& fragSrc) {
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    if(!vertShaderId) {
        LogWarning("[RenderShaderManager::createProgramImpl] Can't create vertext shader");
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
        LogWarning("[RenderShaderManager::createProgramImpl] Can't compile vertex shader: %s", infoBuffer);
        glDeleteShader(vertShaderId);
        return 0;
    }

    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    if(!fragShaderId) {
        LogWarning("[RenderShaderManager::createProgramImpl] Can't create fragment shader");
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
        LogWarning("[RenderShaderManager::createProgramImpl] Can't compile fragment shader: %s", infoBuffer);
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
        LogWarning("[RenderShaderManager::createProgramImpl] Can't link render program: %s", infoBuffer);
        return 0;
    }
    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
    return programId;
}

int RenderShaderManager::createShaderProgram(const std::string& shaderName) {
    auto& shadersTable = Core::GetGlobal<RenderConfig>()->shadersTable;
    if(shadersTable.empty()) {
        LogError("[RenderShaderManager::createShaderProgram] Shaders table not specified");
        return INVALID_PROGRAM_ID;
    }

    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, shadersTable.c_str());
    if(!rootNode) {
        LogError("[RenderShaderManager::createShaderProgram] Can't create shader '%s' from: %s", shaderName, shadersTable);
        return INVALID_PROGRAM_ID;
    }
    for(auto& shaderNode : rootNode) {
        if(shaderName != shaderNode.key()) {
            continue;
        }
        std::string vertShaderPath;
        std::string fragShaderPath;
        shaderNode.read("vert", vertShaderPath);
        shaderNode.read("frag", fragShaderPath);
        if(vertShaderPath.empty()) {
            LogError("[RenderShaderManager::createShaderProgram] Empty vert shader path in shader: %s", shaderName);
            continue;
        }
        if(fragShaderPath.empty()) {
            LogError("[RenderShaderManager::createShaderProgram] Empty frag shader path in shader: %s", shaderName);
            continue;
        }
        auto programId = createProgram(vertShaderPath, fragShaderPath);
        if(!programId) {
            LogError("[RenderShaderManager::createShaderProgram] Can't create render shader '%s' from vert: '%s' and frag: '%s' shaders",
                shaderName, vertShaderPath, fragShaderPath);
            return INVALID_PROGRAM_ID;
        }
        return programId;
    }
    return INVALID_PROGRAM_ID;
}