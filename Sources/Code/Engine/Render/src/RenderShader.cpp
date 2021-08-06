#include "RenderShader.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"

#include <cassert>

RenderShader::RenderShader(int progId) :
    programId(progId) {
    assert(programId && "Invalid program id");
}

RenderShader::~RenderShader() {
}

void RenderShader::bind() {
    glUseProgram(programId);
}

void RenderShader::unbind() {
    glUseProgram(0);
}

void RenderShader::setTexture2d(UniformType varType, int unitId, RenderTexture& tex) {
    setUniform1i(varType, unitId);
    glActiveTexture(GL_TEXTURE0 + unitId);
    tex.bind();
}

void RenderShader::setUniform1i(UniformType varType, int val) {
    glUniform1i(getUniformId(varType), val);
}

void RenderShader::setUniform1f(UniformType varType, float val) {
    glUniform1f(getUniformId(varType), val);
}

void RenderShader::setUniform2f(UniformType varType, const Vec2i& val) {
    Vec2 v;
    v.x = static_cast<float>(val.x);
    v.y = static_cast<float>(val.y);
    glUniform2fv(getUniformId(varType), 1, v.getPtr());
}

void RenderShader::setUniform2f(UniformType varType, const Vec2& val) {
    glUniform2fv(getUniformId(varType), 1, val.getPtr());
}

void RenderShader::setUniformMat4(UniformType varType, const Mat4& mat) {
    glUniformMatrix4fv(getUniformId(varType), 1, false, mat.getPtr());
}

void RenderShader::setUniform4f(UniformType varType, const ColorB& col) {
    ColorF colF = Color::toColorF(col);
    setUniform4f(varType, Vec4(colF.r, colF.g, colF.b, colF.a));
}

void RenderShader::setUniform4f(UniformType varType, const Vec4& vec) {
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
    chachedIds.clear();
    chachedIds.resize(static_cast<int>(UniformType::ENUM_SIZE));
    {
        int uniformId = -1;
        findUniform("CameraMat", uniformId);
        chachedIds[static_cast<int>(UniformType::CameraMat)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform("ModelMat", uniformId);
        chachedIds[static_cast<int>(UniformType::ModelMat)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform("alpha", uniformId);
        chachedIds[static_cast<int>(UniformType::Alpha)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform("tex", uniformId);
        chachedIds[static_cast<int>(UniformType::Texture)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform("color", uniformId);
        chachedIds[static_cast<int>(UniformType::Color)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform("texSize", uniformId);
        chachedIds[static_cast<int>(UniformType::TextureSize)] = uniformId;
    }
    {
        int uniformId = -1;
        findUniform("vertical", uniformId);
        chachedIds[static_cast<int>(UniformType::IsVerticalPass)] = uniformId;
    }
}