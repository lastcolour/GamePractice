#include "RenderGraph/RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Core/ETPrimitives.hpp"

RenderContext::RenderContext() :
    dt(0.f),
    blendingType(RenderBlendingType::NONE),
    mainFBO(nullptr) {
}

RenderContext::~RenderContext() {
}

void RenderContext::setBlending(RenderBlendingType newBlendingType) {
    if(newBlendingType == blendingType) {
        return;
    }
    blendingType = newBlendingType;
    switch(blendingType)
    {
    case RenderBlendingType::NONE:
        glDisable(GL_BLEND);
        break;
    case RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    default:
        break;
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

RenderBlendingType RenderContext::getBlendingType() const {
    return blendingType;
}