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
    void preDraw(RenderState& renderState) override;
    void draw(RenderState& renderState, DrawCmdSlice& slice) override;

public:

    static int GenTextVertData(const std::string& text, const TextMetric& metric, const RenderFont& font, bool alignAtCenter, void* out);

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __DRAW_TEXT_EXECUTOR_HPP__ */