#include "Render/RenderFontSystem.hpp"
#include "ETApplicationInterfaces.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H 

namespace {
    const char* DEFAULT_FACE = nullptr;
}

RenderFontSystem::RenderFontSystem() :
    ftLib(nullptr) {
}

RenderFontSystem::~RenderFontSystem() {
}

bool RenderFontSystem::init() {
    if(FT_Init_FreeType(ftLib)) {
        LogError("[RenderFontSystem::init] Can't init FreeType library");
        return false;
    }
    return false;
}