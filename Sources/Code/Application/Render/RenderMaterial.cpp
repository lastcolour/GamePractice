#include "Render/RenderMaterial.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <cassert>

RenderMaterial::RenderMaterial(int progId) : programId(progId) {
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

void RenderMaterial::setUniformMat4(const std::string& name, const Mat4& mat) {
    GLint uniLoc = -1;
    if(findUniform(name, uniLoc)) {
        glUniformMatrix4fv(uniLoc, 1, false, mat.getPtr());
    }
}

void RenderMaterial::setUniform4f(const std::string& name, const ColorF& col) {
    setUniform4f(name, Vec4(col.r, col.g, col.b, col.a));
}

void RenderMaterial::setUniform4f(const std::string& name, const Vec4& vec) {
    GLint uniLoc = -1;
    if(findUniform(name, uniLoc)) {
        glUniform4fv(uniLoc, 1, vec.getPtr());
    }
}