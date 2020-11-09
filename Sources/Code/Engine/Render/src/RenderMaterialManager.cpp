#include "RenderMaterialManager.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderMaterial.hpp"

#include <algorithm>

namespace {

const int MAX_INFO_BUFF_SIZE = 512u;
const char* MATERIALS = "Render/Materials.json";
const char* SHADERS_ROOT_DIR = "Render/";
const int INVALID_PROGRAM_ID = 0;

} // namespace

RenderMaterialManager::RenderMaterialManager() {
}

RenderMaterialManager::~RenderMaterialManager() {
}

bool RenderMaterialManager::init() {
    ETNode<ETRenderMaterialManager>::connect(getEntityId());
    return true;
}

void RenderMaterialManager::deinit() {
    ETNode<ETRenderMaterialManager>::disconnect();
}

std::shared_ptr<RenderMaterial> RenderMaterialManager::ET_createMaterial(const char* matName) {
    std::string reqMatName = matName;
    if(reqMatName.empty()) {
        LogWarning("[RenderMaterialManager::createMaterial] Can't create material with empty name");
        return nullptr;
    }
    auto it = materials.find(reqMatName);
    if(it != materials.end() && it->second) {
        return it->second;
    }
    auto programId = createMaterialProgram(reqMatName);
    if(!programId) {
        return nullptr;
    }
    std::shared_ptr<RenderMaterial> material(new RenderMaterial(programId));
    materials[reqMatName] = material;
    return material;
}

int RenderMaterialManager::createProgram(const std::string& vertFile, const std::string& fragFile) {
    Buffer buffer;
    ET_SendEventReturn(buffer, &ETAssets::ET_loadAsset, vertFile.c_str());
    if(!buffer) {
        LogWarning("[RenderMaterialManager::createProgram] Can't load vert shader file: %s", vertFile.c_str());
        return 0;
    }
    std::string vertSrc = buffer.acquireString();
    if(vertSrc.empty()) {
        LogWarning("[RenderMaterialManager::createProgram] Loaded empty vert shader source from %s", vertFile.c_str());
        return 0;
    }
    ET_SendEventReturn(buffer, &ETAssets::ET_loadAsset, fragFile.c_str());
    if(!buffer) {
        LogWarning("[RenderMaterialManager::createProgram] Can't load frag shader file: %s", fragFile.c_str());
        return 0;
    }
    std::string fragSrc = buffer.acquireString();
    if(fragSrc.empty()) {
        LogWarning("[RenderMaterialManager::createProgram] Loaded empty fragFile shader source from %s", vertFile.c_str());
        return 0;
    }
    return createProgramImpl(vertSrc, fragSrc);
}

int RenderMaterialManager::createProgramImpl(const std::string& vertSrc, const std::string& fragSrc) {
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    if(!vertShaderId) {
        LogWarning("[RenderMaterialManager::createProgramImpl] Can't create vertext shader");
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
        LogWarning("[RenderMaterialManager::createProgramImpl] Can't compile vertex shader: %s", infoBuffer);
        glDeleteShader(vertShaderId);
        return 0;
    }

    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    if(!fragShaderId) {
        LogWarning("[RenderMaterialManager::createProgramImpl] Can't create fragment shader");
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
        LogWarning("[RenderMaterialManager::createProgramImpl] Can't compile fragment shader: %s", infoBuffer);
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
        LogWarning("[RenderMaterialManager::createProgramImpl] Can't link render program: %s", infoBuffer);
        return 0;
    }
    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
    return programId;
}

int RenderMaterialManager::createMaterialProgram(const std::string& matName) {
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, MATERIALS);
    if(!rootNode) {
        LogError("[RenderMaterialManager::createMaterialProgram] Can't create materials '%s' from: %s", matName, MATERIALS);
        return INVALID_PROGRAM_ID;
    }
    for(auto& matNode : rootNode) {
        if(matName != matNode.key()) {
            continue;
        }
        std::string vertShaderPath;
        std::string fragShaderPath;
        matNode.read("vert", vertShaderPath);
        matNode.read("frag", fragShaderPath);
        if(vertShaderPath.empty()) {
            LogError("[RenderMaterialManager::createMaterialProgram] Empty vert shader path in material: %s", matName);
            continue;
        }
        if(fragShaderPath.empty()) {
            LogError("[RenderMaterialManager::createMaterialProgram] Empty frag shader path in material: %s", matName);
            continue;
        }
        vertShaderPath = SHADERS_ROOT_DIR + vertShaderPath;
        fragShaderPath = SHADERS_ROOT_DIR + fragShaderPath;
        auto programId = createProgram(vertShaderPath, fragShaderPath);
        if(!programId) {
            LogError("[RenderMaterialManager::createMaterialProgram] Can't create render material '%s' from vert: '%s' and frag: '%s' shaders",
                matName, vertShaderPath, fragShaderPath);
            return INVALID_PROGRAM_ID;
        }
        return programId;
    }
    return INVALID_PROGRAM_ID;
}