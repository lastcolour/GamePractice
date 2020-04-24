#include "Render/RenderContext.hpp"
#include "Platforms/OpenGL.hpp"

RenderContext::RenderContext() :
    dt(0.f),
    isSrcMinusAlphaBlendingEnabled(false) {
}

RenderContext::~RenderContext() {
}

void RenderContext::setSrcMinusAlphaBlending(bool flag) {
    if(flag) {
        if(!isSrcMinusAlphaBlendingEnabled) {
            isSrcMinusAlphaBlendingEnabled = true;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    } else {
        if(isSrcMinusAlphaBlendingEnabled) {
            isSrcMinusAlphaBlendingEnabled = false;
            glDisable(GL_BLEND);
        }
    }
}