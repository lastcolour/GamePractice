#ifndef __RENDER_FONT_SYSTEM_HPP__
#define __RENDER_FONT_SYSTEM_HPP__

struct FT_Library;

class RenderFontSystem {
public:
    RenderFontSystem();
    ~RenderFontSystem();
    
    bool init();

private:

    FT_Library* ftLib;
};

#endif /* __RENDER_FONT_SYSTEM_HPP__ */