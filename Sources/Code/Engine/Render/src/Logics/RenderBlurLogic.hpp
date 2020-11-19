#ifndef __RENDER_BLUR_LOGIC_HPP__
#define __RENDER_BLUR_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

class RenderBlurLogic : public RenderNode {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderBlurLogic();
    virtual ~RenderBlurLogic();

    // EntityLogic
    bool init() override;

protected:

    void onSyncWithRender() override;

private:

    int passes;
    int downScaleFactor;
    bool isPassesChanged;
    bool isDownScaleChanged;
};

#endif /* __RENDER_BLUR_LOGIC_HPP__ */