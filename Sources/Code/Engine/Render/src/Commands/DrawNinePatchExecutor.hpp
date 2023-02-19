#ifndef __DRAW_NINE_PATCH_EXECUTOR_HPP__
#define __DRAW_NINE_PATCH_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"

class DrawNinePatchExecutor : public DrawCommandExecutor<DrawNinePatchCmd>
{
public:

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

#endif /* __DRAW_NINE_PATCH_EXECUTOR_HPP__ */