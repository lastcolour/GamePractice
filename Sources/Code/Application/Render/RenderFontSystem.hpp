#ifndef __RENDER_FONT_SYSTEM_HPP__
#define __RENDER_FONT_SYSTEM_HPP__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class RenderFont;

class RenderFontSystem {
public:

    RenderFontSystem();
    ~RenderFontSystem();

    bool init();

    std::shared_ptr<RenderFont> createFont(const std::string& fontName, int fontSize);

private:

    std::shared_ptr<RenderFont> createFontImpl(const std::string& fontName, int fontSize);

private:

    typedef std::weak_ptr<RenderFont> RenderFontPtrT;
    std::unordered_map<std::string, RenderFontPtrT> fonts;

    std::vector<int> characterSet;
    int padding;
};

#endif /* __RENDER_FONT_SYSTEM_HPP__ */