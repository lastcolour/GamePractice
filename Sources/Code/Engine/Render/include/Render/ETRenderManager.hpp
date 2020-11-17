#ifndef __ET_RENDER_MANAGER_HPP__
#define __ET_RENDER_MANAGER_HPP__

#include "Math/Vector.hpp"
#include "Render/RenderCommon.hpp"

#include <memory>

class RenderShader;
class RenderFont;
class RenderTexture;
class RenderGeometry;

struct ETRenderGeometryManager {
    virtual ~ETRenderGeometryManager() = default;
    virtual std::shared_ptr<RenderGeometry> ET_createGeometry(PrimitiveGeometryType geomType) = 0;
};

struct ETRenderFontManager {
    virtual ~ETRenderFontManager() = default;
    virtual std::shared_ptr<RenderFont> ET_getFont(const char* fontName) = 0;
    virtual std::shared_ptr<RenderFont> ET_getDefaultFont() = 0;
};

struct ETRenderTextureManger {
    virtual ~ETRenderTextureManger() = default;
    virtual std::shared_ptr<RenderTexture> ET_createTexture(const char* textureName, ETextureType texType) = 0;
    virtual std::shared_ptr<RenderTexture> ET_createEmptyTexture(const Vec2i& texSize, ETextureType texType) = 0;
};

struct ETRenderShaderManager {
    virtual ~ETRenderShaderManager() = default;
    virtual std::shared_ptr<RenderShader> ET_createShader(const char* shaderName) = 0;
};

#endif /* __ET_RENDER_MANAGER_HPP__ */