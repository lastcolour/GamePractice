#include "RenderShader.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"

#include <cassert>

namespace {

const char* getUniformName(UniformType uniformType) {
    switch(uniformType) {
        case UniformType::CameraMat:
            return "CameraMat";
        case UniformType::ModelMat:
            return "ModelMat";
        case UniformType::Alpha:
            return "alpha";
        case UniformType::Color:
            return "color";
        case UniformType::Texture:
            return "tex";
        case UniformType::TextureSize:
            return "texSize";
        case UniformType::IsVerticalPass:
            return "vertical";
        case UniformType::ENUM_SIZE:
            [[fallthrough]];
        default: 
            assert(false && "Invalid uniform type");
    }
    return nullptr;
}

} // namespace

RenderShader::RenderShader(int progId) :
    programId(progId),
    bound(false) {
    assert(programId && "Invalid program id");
}

RenderShader::~RenderShader() {
}

void RenderShader::bind() {
    assert(!bound && "Shader is already bound");

    bound = true;
    glUseProgram(programId);
}

void RenderShader::unbind() {
    assert(bound && "Shader isn't bound");

    bound = false;
    glUseProgram(0);
}

bool RenderShader::isBound() const {
    return bound;
}

void RenderShader::setTexture2d(UniformType varType, int unitId, RenderTexture& texObj) {
    assert(bound && "Shader isn't bound");

    setUniform1i(varType, unitId);
    glActiveTexture(GL_TEXTURE0 + unitId);
    texObj.bind();
}

void RenderShader::setTexture2d(UniformType varType, int unitId, RenderTexture& texObj, TextureInfo& texInfo) {
    texObj.bind();
    texObj.setTexInfo(texInfo);

    setUniform1i(varType, unitId);
    glActiveTexture(GL_TEXTURE0 + unitId);
}

void RenderShader::setUniform1i(UniformType varType, int val) {
    assert(bound && "Shader isn't bound");

    glUniform1i(getUniformId(varType), val);
}

void RenderShader::setUniform1f(UniformType varType, float val) {
    assert(bound && "Shader isn't bound");

    glUniform1f(getUniformId(varType), val);
}

void RenderShader::setUniform2f(UniformType varType, const Vec2i& val) {
    assert(bound && "Shader isn't bound");

    Vec2 v;
    v.x = static_cast<float>(val.x);
    v.y = static_cast<float>(val.y);
    glUniform2fv(getUniformId(varType), 1, v.getPtr());
}

void RenderShader::setUniform2f(UniformType varType, const Vec2& val) {
    assert(bound && "Shader isn't bound");

    glUniform2fv(getUniformId(varType), 1, val.getPtr());
}

void RenderShader::setUniformMat4(UniformType varType, const Mat4& mat) {
    assert(bound && "Shader isn't bound");

    glUniformMatrix4fv(getUniformId(varType), 1, false, mat.getPtr());
}

void RenderShader::setUniform4f(UniformType varType, const ColorB& col) {
    assert(bound && "Shader isn't bound");

    ColorF colF = Color::toColorF(col);
    setUniform4f(varType, Vec4(colF.r, colF.g, colF.b, colF.a));
}

void RenderShader::setUniform4f(UniformType varType, const Vec4& vec) {
    assert(bound && "Shader isn't bound");

    glUniform4fv(getUniformId(varType), 1, vec.getPtr());
}

int RenderShader::getProgramId() const {
    return programId;
}

void RenderShader::setProgramId(int newProgramId) {
    programId = newProgramId;
}

int RenderShader::getUniformId(UniformType varType) const {
    assert(varType < UniformType::ENUM_SIZE && "Invalid uniform type");
    int varId = chachedIds[static_cast<int>(varType)];
    if(varId == -1) {
        LogError("[RenderShader::getUniformId] Can't find uniform: '%s'", getUniformName(varType));
        assert(false && "Can't find uniform id");
    }
    assert(varId != -1 && "Invalid uniform id");
    return varId;
}

bool RenderShader::findUniform(const char* name, int& resUniLoc) const {
    GLint uniLoc = glGetUniformLocation(programId, name);
    if(uniLoc == -1) {
        return false;
    }
    resUniLoc = uniLoc;
    return true;
}

void RenderShader::cacheUniformIds() {
    assert(bound && "Shader isn't bound");

    chachedIds.clear();
    chachedIds.resize(static_cast<int>(UniformType::ENUM_SIZE));
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::CameraMat), uniformId);
        chachedIds[static_cast<int>(UniformType::CameraMat)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::ModelMat), uniformId);
        chachedIds[static_cast<int>(UniformType::ModelMat)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::Alpha), uniformId);
        chachedIds[static_cast<int>(UniformType::Alpha)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::Texture), uniformId);
        chachedIds[static_cast<int>(UniformType::Texture)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::Color), uniformId);
        chachedIds[static_cast<int>(UniformType::Color)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::TextureSize), uniformId);
        chachedIds[static_cast<int>(UniformType::TextureSize)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform(getUniformName(UniformType::IsVerticalPass), uniformId);
        chachedIds[static_cast<int>(UniformType::IsVerticalPass)] = uniformId;
    }
}