#ifndef __RENDER_FONT_MANAGER_HPP__
#define __RENDER_FONT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderInterfaces.hpp"

class FontDescription;
class RenderFont;

class RenderFontManager : public SystemLogic,
    public ETNode<ETRenderFontManager>,
    public ETNode<ETRenderContextEvents> {
public:

    RenderFontManager();
    virtual ~RenderFontManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderContextEvents
    void ET_onContextCreated() override;
    void ET_onContextDestroyed() override;

    // ETRenderFontManager
    std::shared_ptr<RenderFont> ET_createFont(EFontType fontType) override;

private:

    std::shared_ptr<RenderFont> createFont(const FontDescription& fontDecr);
    std::shared_ptr<RenderFont> createFontImpl(const FontDescription& fontDecr);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderFont>> fonts;
    std::vector<int> characterSet;
};

#endif /* __RENDER_FONT_MANAGER_HPP__ */