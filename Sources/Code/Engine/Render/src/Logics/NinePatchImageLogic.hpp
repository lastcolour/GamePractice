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
    Vec2 ET_getPatchesTextureCoords() const override;
    void ET_setPatchesTextureCoords(const Vec2& newPatchesTextureCoords) override;
    void ET_setPatchSize(float NewSize) override;
    float ET_getPatchSize() const override;
    void ET_setPatchSizeType(ENinePatchSizeType newPatchSizeType) override;
    ENinePatchSizeType ET_getPatchSizeType() const override;
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

    Vec2 patchesTextureCoords;
    float patchSize;
    ENinePatchSizeType patchSizeType;
};

#endif /* __NINE_PATCH_IMAGE_LOGIC_HPP__ */