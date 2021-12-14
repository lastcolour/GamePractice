#ifndef __DRAW_TEXT_EXECUTOR_HPP__
#define __DRAW_TEXT_EXECUTOR_HPP__

#include "Commands/DrawCommandExecutor.hpp"
#include "RenderShader.hpp"
#include "RenderGeometry.hpp"

class DrawTextExecutor : public DrawCommandExecutor<DrawTextCmd> {
public:

    DrawTextExecutor();
    virtual ~DrawTextExecutor();

    // DrawCommandExecutor
    bool init() override;
    void deinit() override;
    void preDraw() override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
    std::vector<Vec4> vertData;
};

#endif /* __DRAW_TEXT_EXECUTOR_HPP__ */