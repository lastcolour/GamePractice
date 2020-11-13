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
    bool init() override;

    // ETRenderImageLogic
    void ET_setImage(const char* imageName) override;

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

protected:

    void onSyncWithRender() override;

protected:

    std::string image;
    Vec2 size;
    bool isImageChanged;
    bool isSizeChanged;
};

#endif /* __RENDER_IMAGE_LOGIC_HPP__ */