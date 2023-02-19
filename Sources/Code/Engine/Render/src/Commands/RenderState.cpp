#include "Commands/RenderState.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

namespace {

const int EXTRA_FBOS_COUNT = 2;

} // namespace

RenderState::RenderState() :
    mainFBO(nullptr),
    blendOpPair(BlendOpPair{EBlendOp::NONE, EBlendOp::NONE}) {
}

RenderState::~RenderState() {
}

bool RenderState::init() {
    ET_SendEventReturn(mainFBO, &ETRenderTextureManager::ET_createFramebuffer, EFramebufferType::Color_Depth_Stencil, Vec2i(1));
    if(!mainFBO) {
        LogError("[RenderState::init] Can't create main framebuffer");
        return false;
    }

    mainFBO->color0.bind();
    mainFBO->color0.setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
    mainFBO->color0.unbind();

    for(int i = 0; i < EXTRA_FBOS_COUNT; ++i) {
        std::shared_ptr<RenderFramebuffer> extraFBO;
        ET_SendEventReturn(extraFBO, &ETRenderTextureManager::ET_createFramebuffer, EFramebufferType::Color, Vec2i(1));
        if(!extraFBO) {
            LogError("[RenderState::init] Can't create extra framebuffer");
            return false;
        }

        extraFBO->color0.bind();
        extraFBO->color0.setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
        extraFBO->color0.unbind();

        extraFBOs.push_back(extraFBO);
    }

    ET_SendEventReturn(copyFBOShader, &ETRenderShaderManager::ET_createShader, "draw_fbo");
    if(!copyFBOShader) {
        return false;
    }
    ET_SendEventReturn(copyFBOGeom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3_Tex);
    if(!copyFBOGeom) {
        return false;
    }
    return true;
}

void RenderState::setBlenMode(EBlendMode blendMode, bool preMultAlpha) {
    auto newBlendOpPair = RenderUtils::GetBlendOpPair(blendMode, preMultAlpha);
    setBlendOpPair(newBlendOpPair);
}

void RenderState::setBlendOpPair(const BlendOpPair& newBlendOpPair) {
    if(blendOpPair.dst == newBlendOpPair.dst &&
        blendOpPair.src == newBlendOpPair.src) {
            return;
    }
    blendOpPair = newBlendOpPair;
    if(blendOpPair.dst == EBlendOp::NONE || blendOpPair.src == EBlendOp::NONE) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(
            RenderUtils::GetGLBlendMode(blendOpPair.src),
            RenderUtils::GetGLBlendMode(blendOpPair.dst));
    }
}

void RenderState::setStencilState(const StencilWirteReadData& newStencilState) {
    if((stencilState.opType == newStencilState.opType) &&
        (stencilState.refVal == newStencilState.refVal)) {
        return;
    }
    bool isStencilEnabled = stencilState.opType != EStencilOpType::Disabled;
    stencilState = newStencilState;
    switch(stencilState.opType) {
        case EStencilOpType::Disabled: {
            glDisable(GL_STENCIL_TEST);
            break;
        }
        case EStencilOpType::Read: {

            if(!isStencilEnabled) {
                glEnable(GL_STENCIL_TEST);
            }
            glStencilMask(0x00);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            glStencilFunc(GL_EQUAL, stencilState.refVal, 0xFF);
            break;
        }
        case EStencilOpType::ReadIncrease: {
            if(!isStencilEnabled) {
                glEnable(GL_STENCIL_TEST);
            }
            glStencilMask(0xFF);
            glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
            glStencilFunc(GL_EQUAL, stencilState.refVal, 0xFF);
            break;
        }
        case EStencilOpType::Write: {
            if(!isStencilEnabled) {
                glEnable(GL_STENCIL_TEST);
            }
            glStencilMask(0xFF);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, stencilState.refVal, 0xFF);
            break;
        }
        default: {
            assert(false && "Invalid Stecil OP type");
        }
    }
}

bool RenderState::copyFBOtoFBO(RenderFramebuffer& from, RenderFramebuffer& to) {
    if(!copyFBOGeom || !copyFBOShader) {
        LogError("[RenderState::copyFBOtoFBO] Copy 'geom' or 'shader' doesn't exist");
        return false;
    }

    if(&from == &to) {
        LogError("[RenderState::copyFBOtoFBO] Can't copy to the same FBO");
        return false;
    }

    to.bind();
    copyFBOShader->bind();
    copyFBOShader->setTexture2d(UniformType::Texture, 0, from.color0);
    {
        copyFBOGeom->bind();
        copyFBOGeom->drawTriangles(0, 6);
        copyFBOGeom->unbind();
    }
    copyFBOShader->unbind();
    to.unbind();

    if(auto err = RenderUtils::GetGLError()) {
        LogError("[RenderState::copyFBOtoFBO] Can't copy FBO to FBO (Error: %s)", err);
        return false;
    }

    return true;
}

bool RenderState::copyFBOtoDefaultFBO(RenderFramebuffer& from) {
   if(!copyFBOGeom || !copyFBOShader) {
       LogError("[RenderState::copyFBOtoDefaultFBO] Copy 'geom' or 'shader' doesn't exist");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    copyFBOShader->bind();
    copyFBOShader->setTexture2d(UniformType::Texture, 0, from.color0);
    {
        copyFBOGeom->bind();
        copyFBOGeom->drawTriangles(0, 6);
        copyFBOGeom->unbind();
    }
    copyFBOShader->unbind();

    if(auto err = RenderUtils::GetGLError()) {
        LogError("[RenderState::copyFBOtoDefaultFBO] Can't copy FBO to default (Error: %s)", err);
        return false;
    }

    return true;
}

void RenderState::startCommand(const DrawCmd& drawCmd) {
    setStencilState(drawCmd.stencilData);
    setBlendOpPair(drawCmd.blendOpPair);
}