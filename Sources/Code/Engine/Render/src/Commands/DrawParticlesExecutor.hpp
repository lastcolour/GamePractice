#ifndef __DRAW_PARTICLES_EXECUTOR_HPP__
#define __DRAW_PARTICLES_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"

class DrawParticlesExecutor : public DrawCommandExecutor<DrawParticlesCmd> {
public:

    DrawParticlesExecutor();
    virtual ~DrawParticlesExecutor();

    // DrawCommandExecutor
    bool init() override;
    void deinit() override;
    void registerCmdForDraw(DrawCmd* cmd) override;
    void preDraw(RenderState& renderState) override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_PARTICLES_EXECUTOR_HPP__ */