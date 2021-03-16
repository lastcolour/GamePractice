#include "RenderGraph/RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderCamera.hpp"

#include <cassert>

namespace {

GLenum getGLBlendType(BlendType blenType) {
    GLenum resBlendMode = GL_ONE;
    switch(blenType) {
        case BlendType::NONE: {
            assert(false && "Invalid blend mode");
        }
        case BlendType::ZERO: {
            resBlendMode = GL_ZERO;
            break;
        }
        case BlendType::ONE: {
            resBlendMode = GL_ONE;
            break;
        }
        case BlendType::SRC_COLOR: {
            resBlendMode = GL_SRC_COLOR;
            break;
        }
        case BlendType::ONE_MINUS_SRC_COLOR: {
            resBlendMode = GL_ONE_MINUS_SRC_COLOR;
            break;
        }
        case BlendType::DST_COLOR: {
            resBlendMode = GL_DST_COLOR;
            break;
        }
        case BlendType::ONE_MINUS_DST_COLOR: {
            resBlendMode = GL_ONE_MINUS_DST_COLOR;
            break;
        }
        case BlendType::SRC_ALPHA: {
            resBlendMode = GL_SRC_ALPHA;
            break;
        }
        case BlendType::ONE_MINUS_SRC_ALPHA: {
            resBlendMode = GL_ONE_MINUS_SRC_ALPHA;
            break;
        }
        case BlendType::DST_ALPHA: {
            resBlendMode = GL_DST_ALPHA;
            break;
        }
        case BlendType::ONE_MINUS_DST_ALPHA: {
            resBlendMode = GL_ONE_MINUS_DST_ALPHA;
            break;
        }
        case BlendType::CONSTANT_COLOR: {
            resBlendMode = GL_CONSTANT_COLOR;
            break;
        }
        case BlendType::ONE_MINUS_CONSTANT_COLOR: {
            resBlendMode = GL_ONE_MINUS_CONSTANT_COLOR;
            break;
        }
        case BlendType::CONSTANT_ALPHA: {
            resBlendMode = GL_CONSTANT_ALPHA;
            break;
        }
        case BlendType::ONE_MINUS_CONSTANT_ALPHA: {
            resBlendMode = GL_ONE_MINUS_CONSTANT_ALPHA;
            break;
        }
        default: {
            assert(false && "Invalid blend mode");
        }
    }
    return resBlendMode;
}

} // namespace

RenderContext::RenderContext() :
    dt(0.f),
    mainFBO(nullptr),
    blendMode(BlendMode{BlendType::NONE, BlendType::NONE}) {
}

RenderContext::~RenderContext() {
}

void RenderContext::setBlending(const BlendMode& newBlendMode) {
    if(blendMode.dst == newBlendMode.dst &&
        blendMode.src == newBlendMode.src) {
            return;
    }
    blendMode = newBlendMode;
    if(blendMode.dst == BlendType::NONE || blendMode.src == BlendType::NONE) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(
            getGLBlendType(blendMode.src),
            getGLBlendType(blendMode.dst));
    }
}

void RenderContext::setStencilState(const StencilWirteReadData& newStencilState) {
    if(stencilState.mode == newStencilState.mode &&
        stencilState.refVal == newStencilState.refVal) {
        return;
    }
    bool isStencilEnabled = stencilState.mode != EStencilOpType::Disabled;
    stencilState = newStencilState;
    switch(stencilState.mode) {
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
    }
}