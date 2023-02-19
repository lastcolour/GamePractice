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

    // EntityLogic
    void onTransformChanged(const Transform& newTm) override;

    // ETNinePatchImageLogic
    Vec2 ET_getPatches() const override;
    void ET_setPatches(const Vec2& patches) override;
    Vec2 ET_getPatchesVertCoord() const override;

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;

    // ETRenderImageLogic
    void ET_setTextureInfo(const TextureInfo& newTexture) override;

protected:

    // DrawCommandProxy
    void onInit() override;
    Mat4 calcModelMat() const override;

private:

    float horizontal;
    float vertical;
};

#endif /* __NINE_PATCH_IMAGE_LOGIC_HPP__ */