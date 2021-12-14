#ifndef __DRAW_TEXTURED_QUAD_EXECUTOR_HPP__
#define __DRAW_TEXTURED_QUAD_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"

class DrawTexturedQuadExecutor : public DrawCommandExecutor<DrawTexturedQuadCmd> {
public:

    DrawTexturedQuadExecutor();
    virtual ~DrawTexturedQuadExecutor();

    // DrawCommandExecutor
    bool init() override;
    void deinit() override;
    void preDraw() override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;
    void registerCmdForDraw(DrawCmd* cmd) override;

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_TEXTURED_QUAD_EXECUTOR_HPP__ */