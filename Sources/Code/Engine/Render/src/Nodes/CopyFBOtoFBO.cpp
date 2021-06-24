#include "Nodes/CopyFBOtoFBO.hpp"
#include "Render/ETRenderManager.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderShader.hpp"
#include "RenderGeometry.hpp"
#include "RenderUtils.hpp"

CopyFBOtoFBO::CopyFBOtoFBO() {
}

CopyFBOtoFBO::~CopyFBOtoFBO() {
}

bool CopyFBOtoFBO::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "draw_fbo");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, PrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

bool CopyFBOtoFBO::canCopy() const {
    if(!shader) {
        return false;
    }
    if(!geom) {
        return false;
    }
    return true;
}

bool CopyFBOtoFBO::copy(RenderFramebuffer& from, RenderFramebuffer& to) {
    if(!canCopy()) {
        return false;
    }

    if(&from == &to) {
        LogError("[CopyFBOtoFBO::copy] Can't copy to the same FBO");
        return false;
    }

    to.bind();

    shader->bind();
    shader->setTexture2d(UniformType::Texture, 0, from.color0);
    geom->drawTriangles();
    shader->unbind();

    to.unbind();

    if(auto err = RenderUtils::GetGLError()) {
        LogError("[CopyFBOtoFBO::copy] Can't copy FBO to FBO (Error: %s)", err);
        return false;
    }

    return true;
}

bool CopyFBOtoFBO::copyToDefault(RenderFramebuffer& from) {
    if(!canCopy()) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader->bind();
    shader->setTexture2d(UniformType::Texture, 0, from.color0);
    geom->drawTriangles();
    shader->unbind();

    if(auto err = RenderUtils::GetGLError()) {
        LogError("[CopyFBOtoFBO::copyToDefault] Can't copy FBO to default (Error: %s)", err);
        return false;
    }

    return true;
}