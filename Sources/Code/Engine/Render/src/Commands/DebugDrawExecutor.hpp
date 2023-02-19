#ifndef __DEBUG_DRAW_EXECUTOR_HPP__
#define __DEBUG_DRAW_EXECUTOR_HPP__

#include "Commands/DebugCmdCollector.hpp"
#include "Commands/RenderState.hpp"

class DebugDrawExecutor {
public:

    DebugDrawExecutor();
    virtual ~DebugDrawExecutor();

    bool init();
    void deinit();
    void preRender();
    void draw(RenderState& renderState);

private:

    void drawQuads(RenderState& renderState);
    void drawLines(RenderState& renderState);
    void drawTexts(RenderState& renderState);

private:

    std::shared_ptr<RenderShader> lineShader;
    std::shared_ptr<RenderGeometry> lineGeom;

    std::shared_ptr<RenderShader> quadShader;
    std::shared_ptr<RenderGeometry> quadGeom;

    std::shared_ptr<RenderShader> textShader;
    std::shared_ptr<RenderGeometry> textGeom;
    std::shared_ptr<RenderFont> textFont;

    DebugCmdCollector cmdCollector;
};

#endif /* __DEBUG_DRAW_EXECUTOR_HPP__ */