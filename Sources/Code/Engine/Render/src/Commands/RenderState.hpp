#ifndef __RENDER_STATE_HPP__
#define __RENDER_STATE_HPP__

#include "Commands/Commands.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderShader.hpp"
#include "RenderGeometry.hpp"

class RenderState {
public:

    RenderState();
    ~RenderState();

    bool init();

    void startCommand(const DrawCmd& drawCmd);
    void setStencilState(const StencilWirteReadData& newStencilState);
    void setBlendOpPair(const BlendOpPair& newBlendOpPair);
    void setBlenMode(EBlendMode blendMode, bool preMultAlpha);

    bool copyFBOtoFBO(RenderFramebuffer& from, RenderFramebuffer& to);
    bool copyFBOtoDefaultFBO(RenderFramebuffer& from);

public:

    Mat4 proj2dMat;
    std::shared_ptr<RenderFramebuffer> mainFBO;
    std::vector<std::shared_ptr<RenderFramebuffer>> extraFBOs;

private:

    StencilWirteReadData stencilState;
    BlendOpPair blendOpPair;
    std::shared_ptr<RenderGeometry> copyFBOGeom;
    std::shared_ptr<RenderShader> copyFBOShader;
};

#endif /* __RENDER_STATE_HPP__ */