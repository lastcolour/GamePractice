#ifndef __NINE_PATCH_IMAGE_LOGIC_HPP__
#define __NINE_PATCH_IMAGE_LOGIC_HPP__

#include "Logics/RenderImageLogic.hpp"

class NinePatchImageLogic : public RenderImageLogic {
public:

    static void Reflect(ReflectContext& ctx);

public:

    NinePatchImageLogic();
    virtual ~NinePatchImageLogic();

protected:

    // RenderNode
    void onInit() override;

private:

    float horizontal;
    float vertical;
    float patchScale;
};

#endif /* __NINE_PATCH_IMAGE_LOGIC_HPP__ */