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
    void registerCmdForDraw(DrawCmd* cmd) override;
    void preDraw(RenderState& renderState) override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

private:

    std::shared_ptr<RenderShader> gradShader;
    std::shared_ptr<RenderShader> imgShader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_TEXTURED_QUAD_EXECUTOR_HPP__ */