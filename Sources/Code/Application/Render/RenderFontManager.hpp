#ifndef __RENDER_FONT_MANAGER_HPP__
#define __RENDER_FONT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class RenderFont;

class RenderFontManager : public SystemLogic,
    public ETNode<ETRenderFontManager> {
public:

    RenderFontManager();
    virtual ~RenderFontManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderFontManager
    std::shared_ptr<RenderFont> ET_createDefaultFont() override;

private:

    std::shared_ptr<RenderFont> createFont(const char* fontName, int fontSize);
    std::shared_ptr<RenderFont> createFontImpl(const char* fontName, int fontSize);

private:

    typedef std::weak_ptr<RenderFont> RenderFontPtrT;
    std::unordered_map<std::string, RenderFontPtrT> fonts;

    std::vector<int> characterSet;
    int padding;
};

#endif /* __RENDER_FONT_MANAGER_HPP__ */