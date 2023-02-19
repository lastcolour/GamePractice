#ifndef __ET_RENDER_MANAGER_HPP__
#define __ET_RENDER_MANAGER_HPP__

#include "Render/RenderCommon.hpp"

class RenderShader;
class RenderFont;
class RenderTexture;
class RenderGeometry;
class RenderFramebuffer;

struct ETRenderGeometryManager {
    virtual ~ETRenderGeometryManager() = default;
    virtual std::shared_ptr<RenderGeometry> ET_createGeometry(EPrimitiveGeometryType geomType) = 0;
};

struct ETRenderFontManager {
    virtual ~ETRenderFontManager() = default;
    virtual std::shared_ptr<RenderFont> ET_createFont(EFontType fontType) = 0;
};

struct ETRenderTextureManager {
    virtual ~ETRenderTextureManager() = default;
    virtual std::shared_ptr<RenderTexture> ET_createFromFile(const char* fileName, ETextureDataType type) = 0;
    virtual std::shared_ptr<RenderTexture> ET_createTexture(ETextureDataType type, const Vec2i& size) = 0;
    virtual std::shared_ptr<RenderFramebuffer> ET_createFramebuffer(EFramebufferType type, const Vec2i& size) = 0;
};

struct ETRenderShaderManager {
    virtual ~ETRenderShaderManager() = default;
    virtual std::shared_ptr<RenderShader> ET_createShader(const char* shaderName) = 0;
};

#endif /* __ET_RENDER_MANAGER_HPP__ */