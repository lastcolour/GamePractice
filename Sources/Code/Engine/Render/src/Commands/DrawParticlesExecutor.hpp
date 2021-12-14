#ifndef __DRAW_PARTICLES_EXECUTOR_HPP__
#define __DRAW_PARTICLES_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"

class DrawParticlesExecutor : public DrawCommandExecutor<DrawParticlesCmd> {
public:

    DrawParticlesExecutor();
    virtual ~DrawParticlesExecutor();

    void update(float renderDt);

    // DrawCommandExecutor
    bool init() override;
    void deinit() override;
    void preDraw() override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_PARTICLES_EXECUTOR_HPP__ */