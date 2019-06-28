#include "Render/RenderMaterial.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <cassert>

RenderMaterial::RenderMaterial(int progId) :
    activeTexUnitId(0),
    programId(progId) {
    assert(programId && "Invalid program id");
}

RenderMaterial::~RenderMaterial() {
    glDeleteProgram(programId);
}

void RenderMaterial::bind() {
    glUseProgram(programId);
}

void RenderMaterial::unbind() {
    glUseProgram(0);
    activeTexUnitId = 0;
}

void RenderMaterial::setTexture2D(const std::string& name, int texId) {
    setUniform1i(name, activeTexUnitId);
    glActiveTexture(GL_TEXTURE0 + activeTexUnitId);
    glBindTexture(GL_TEXTURE_2D, texId);
    ++activeTexUnitId;
}

bool RenderMaterial::findUniform(const std::string& name, int& resUniLoc) const {
    GLint uniLoc = glGetUniformLocation(programId, name.c_str());
    if(uniLoc == -1) {
        LogError("[RenderMaterial::findUniform] Can't find uniform: %s", name);
        return false;
    }
    resUniLoc = uniLoc;
    return true;
}

void RenderMaterial::setUniform1i(const std::string& name, int val) {
    GLint uniLoc = -1;
    if(findUniform(name, uniLoc)) {
        glUniform1i(uniLoc, val);
    }
}

void RenderMaterial::setUniformMat4(const std::string& name, const Mat4& mat) {
    GLint uniLoc = -1;
    if(findUniform(name, uniLoc)) {
        glUniformMatrix4fv(uniLoc, 1, false, mat.getPtr());
    }
}

void RenderMaterial::setUniform4f(const std::string& name, const ColorB& col) {
    ColorF colF = col.getColorF();
    setUniform4f(name, Vec4(colF.r, colF.g, colF.b, colF.a));
}

void RenderMaterial::setUniform4f(const std::string& name, const Vec4& vec) {
    GLint uniLoc = -1;
    if(findUniform(name, uniLoc)) {
        glUniform4fv(uniLoc, 1, vec.getPtr());
    }
}