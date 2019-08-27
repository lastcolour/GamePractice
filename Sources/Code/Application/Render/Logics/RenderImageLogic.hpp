#ifndef __RENDER_IMAGE_LOGIC_HPP__
#define __RENDER_IMAGE_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class RenderImageLogic : public EntityLogic,
    public ETNode<ETRenderEvents>,
    public ETNode<ETRenderImageLogic> {
public:

    RenderImageLogic();
    virtual ~RenderImageLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;
    void ET_onRenderPortResized() override { }

    // ETRenderImageLogic
    void ET_setImage(const char* imageName) override;
    void ET_setMaterial(const char* matName) override;
    Vec2i ET_getOrigSize() const override;
    Vec2i ET_getSize() const override;
    void ET_setScale(const Vec2& newScale) override; 

private:

    Mat4 getModelMat() const;
    void updateScale();

private:

    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;
    std::shared_ptr<RenderTexture> tex;
    Vec2 imageScale;
    Vec2 texScale;
};

#endif /* __RENDER_IMAGE_LOGIC_HPP__ */