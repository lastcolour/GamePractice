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

    void onSyncWithRender() override;

private:

    float left;
    float right;
    float top;
    float down;
    bool isPatchChanged;
};

#endif /* __NINE_PATCH_IMAGE_LOGIC_HPP__ */