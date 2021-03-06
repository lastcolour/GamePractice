#ifndef __RENDER_IMAGE_LOGIC_HPP__
#define __RENDER_IMAGE_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

class ReflectContext;

class RenderImageLogic : public RenderNode,
    public ETNode<ETRenderImageLogic>,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderImageLogic();
    RenderImageLogic(RenderNodeType nodeType);
    virtual ~RenderImageLogic();

    // EntityLogic
    void init() override;

    // ETRenderImageLogic
    void ET_setImage(const char* imageName) override;
    void ET_setTintColor(const ColorB& newTintColor) override;

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

protected:

    void onSyncWithRender() override;

protected:

    std::string image;
    Vec2 size;
    ColorB tintColor;
    bool isImageChanged;
    bool isSizeChanged;
    bool isTintColorChanged;
};

#endif /* __RENDER_IMAGE_LOGIC_HPP__ */