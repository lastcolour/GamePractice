#ifndef __DEBUG_RENDER_HPP__
#define __DEBUG_RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETDebugRender.hpp"
#include "Debug/DeubgCommands.hpp"

#include <vector>
#include <mutex>

class DebugRender : public SystemLogic,
    public ETNode<ETDebugRender> {
public:

    DebugRender();
    virtual ~DebugRender();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETDebugRender
    void ET_drawLine(const Vec2i& startPt, const ColorB& startCol, const Vec2i& endPt, const Vec2i& endCol, int width) override;
    void ET_drawQuad(const AABB2Di& box, const ColorB& col) override;
    void ET_drawText(const Vec2i& pt, float size, const char* text) override;

private:


};

#endif /* __DEBUG_RENDER_HPP__ */