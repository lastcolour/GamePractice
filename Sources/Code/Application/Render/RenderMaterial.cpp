#include "Render/RenderMaterial.hpp"

#include "Logger.hpp"

#include <cassert>

RenderMaterial::RenderMaterial(GLuint progId) : programId(progId) {
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

void RenderMaterial::setUniform(const std::string& name, const ColorF& col) {
    auto uniLoc = glGetUniformLocation(programId, name.c_str());
    if(uniLoc == -1) {
        LogError("[RenderMaterial::setUniform] Can't get access to uniform: %s", name);
        return;
    }
    glUniform3fv(uniLoc, 1, col.getPtr());
}