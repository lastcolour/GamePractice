#include "Render/RenderMaterialManager.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "Platforms/OpenGL.hpp"
#include "Render/RenderMaterial.hpp"

#include <algorithm>

namespace {
    const int MAX_INFO_BUFF_SIZE = 255u;
    const char* MATERIALS = "Render/Materials.json";
    const char* SHADERS_ROOT_DIR = "Render/";
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
    std::transform(reqMatName.begin(), reqMatName.end(), reqMatName.begin(), tolower);
    if(reqMatName.empty()) {
        LogError("[RenderMaterialManager::createMaterial] Can't create material with empty name");
        return nullptr;
    }
    auto it = materials.find(reqMatName);
    if(it != materials.end() && !it->second.expired()) {
        return it->second.lock();
    }
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, MATERIALS);
    if(!rootNode) {
        LogError("[RenderMaterialManager::createMaterial] Can't create materials '%s' from: %s", matName, MATERIALS);
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
            LogError("[RenderMaterialManager::createMaterial] Empty vert shader path in material: %s", reqMatName);
            continue;
        }
        if(fragShaderPath.empty()) {
            LogError("[RenderMaterialManager::createMaterial] Empty frag shader path in material: %s", reqMatName);
            continue;
        }
        vertShaderPath = SHADERS_ROOT_DIR + vertShaderPath;
        fragShaderPath = SHADERS_ROOT_DIR + fragShaderPath;
        auto program = createProgram(vertShaderPath, fragShaderPath);
        if(!program) {
            LogError("[RenderMaterialManager::createProgram] Can't create render material '%s' from vert: '%s' and frag: '%s' shaders",
                reqMatName, vertShaderPath, fragShaderPath);
            return nullptr;
        }
        LogDebug("[RenderMaterialManager::createMaterial] Load material: '%s'", reqMatName);
        std::shared_ptr<RenderMaterial> material(new RenderMaterial(program));
        materials[reqMatName] = material;
        return material;
    }
    return nullptr;
}

int RenderMaterialManager::createProgram(const std::string& vertFile, const std::string& fragFile) {
    Buffer buffer;
    ET_SendEventReturn(buffer, &ETAssets::ET_loadAsset, vertFile.c_str());
    if(!buffer) {
        LogError("[RenderMaterialManager::createProgram] Can't load vert shader file: %s", vertFile.c_str());
        return 0;
    }
    std::string vertSrc = buffer.getString();
    if(vertSrc.empty()) {
        LogError("[RenderMaterialManager::createProgram] Loaded empty vert shader source from %s", vertFile.c_str());
        return 0;
    }
    ET_SendEventReturn(buffer, &ETAssets::ET_loadAsset, fragFile.c_str());
    if(!buffer) {
        LogError("[RenderMaterialManager::createProgram] Can't load frag shader file: %s", fragFile.c_str());
        return 0;
    }
    std::string fragSrc = buffer.getString();
    if(fragSrc.empty()) {
        LogError("[RenderMaterialManager::createProgram] Loaded empty fragFile shader source from %s", vertFile.c_str());
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