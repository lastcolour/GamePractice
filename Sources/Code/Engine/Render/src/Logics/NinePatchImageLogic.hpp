#ifndef __NINE_PATCH_IMAGE_LOGIC_HPP__
#define __NINE_PATCH_IMAGE_LOGIC_HPP__

#include "Logics/RenderImageLogic.hpp"

class NinePatchImageLogic : public RenderImageLogic,
    public ETNode<ETNinePatchImageLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    NinePatchImageLogic();
    virtual ~NinePatchImageLogic();

    // ETNinePatchImageLogic
    Vec2 ET_getPatches() const override;
    void ET_setPatches(const Vec2& patches) override;
    Vec2 ET_getPatchesVertCoord() const override;

protected:

    // DrawCommandProxy
    void onInit() override;

private:

    float horizontal;
    float vertical;
    float patchScale;
};

#endif /* __NINE_PATCH_IMAGE_LOGIC_HPP__ */