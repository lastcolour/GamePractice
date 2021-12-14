#ifndef __DRAW_BLUR_EXECUTOR_HPP__
#define __DRAW_BLUR_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"

class DrawBlurExecutor : public DrawCommandExecutor<DrawBlurCmd> {
public:

    DrawBlurExecutor();
    virtual ~DrawBlurExecutor();

    // DrawCommandExecutor
    bool init() override;
    void deinit() override;
    void preDraw() override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

private:

    Vec2i size;
    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_BLUR_EXECUTOR_HPP__ */