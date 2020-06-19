#ifndef __RENDER_FONT_MANAGER_HPP__
#define __RENDER_FONT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderManager.hpp"

class RenderFont;

class RenderFontManager : public SystemLogic,
    public ETNode<ETRenderFontManager>,
    public ETNode<ETRenderResourceManager> {
public:

    RenderFontManager();
    virtual ~RenderFontManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderFontManager
    std::shared_ptr<RenderFont> ET_createFont(const char* fontName) override;
    std::shared_ptr<RenderFont> ET_createDefaultFont() override;

    // ETRenderResourceManager
    void ET_forgetResoruces() override;
    void ET_cleanUnused() override;

private:

    std::shared_ptr<RenderFont> createFont(const char* fontName, int fontSize);
    std::shared_ptr<RenderFont> createFontImpl(const char* fontName, int fontSize);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderFont>> fonts;
    std::vector<int> characterSet;
    int padding;
};

#endif /* __RENDER_FONT_MANAGER_HPP__ */