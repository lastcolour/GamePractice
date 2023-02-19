#ifndef __RENDER_BLUR_LOGIC_HPP__
#define __RENDER_BLUR_LOGIC_HPP__

#include "Logics/DrawCommandProxy.hpp"

class RenderBlurLogic : public DrawCommandProxy {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderBlurLogic();
    virtual ~RenderBlurLogic();

protected:

    // DrawCommandProxy
    void onInit() override;
    Mat4 calcModelMat() const override;

private:

    int passes;
    int downScaleFactor;
};

#endif /* __RENDER_BLUR_LOGIC_HPP__ */