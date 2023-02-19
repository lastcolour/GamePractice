#ifndef __DRAW_COLORED_QUAD_EXECUTOR_HPP__
#define __DRAW_COLORED_QUAD_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"

class DrawColoredQuadExecutor : public DrawCommandExecutor<DrawColoredQuadCmd> {
public:

    DrawColoredQuadExecutor();
    virtual ~DrawColoredQuadExecutor();

    // DrawCommandExecutor
    bool init() override;
    void deinit() override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_COLORED_QUAD_EXECUTOR_HPP__ */