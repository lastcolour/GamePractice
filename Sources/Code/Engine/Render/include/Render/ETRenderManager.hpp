#ifndef __ET_RENDER_MANAGER_HPP__
#define __ET_RENDER_MANAGER_HPP__

#include "Math/Vector.hpp"

#include <memory>

class RenderShader;
class RenderFont;
class RenderTexture;
class RenderGeometry;
class RenderFramebuffer;

enum class ETextureType;
enum class EFramebufferType;
enum class PrimitiveGeometryType;

struct ETRenderGeometryManager {
    virtual ~ETRenderGeometryManager() = default;
    virtual std::shared_ptr<RenderGeometry> ET_createGeometry(PrimitiveGeometryType geomType) = 0;
};

struct ETRenderFontManager {
    virtual ~ETRenderFontManager() = default;
    virtual std::shared_ptr<RenderFont> ET_getDefaultFont() = 0;
};

struct ETRenderTextureManger {
    virtual ~ETRenderTextureManger() = default;
    virtual std::shared_ptr<RenderTexture> ET_createFromImage(const char* imageName, ETextureType type) = 0;
    virtual std::shared_ptr<RenderTexture> ET_createTexture(ETextureType type) = 0;
    virtual std::shared_ptr<RenderFramebuffer> ET_createFramebuffer(EFramebufferType type) = 0;
};

struct ETRenderShaderManager {
    virtual ~ETRenderShaderManager() = default;
    virtual std::shared_ptr<RenderShader> ET_createShader(const char* shaderName) = 0;
};

#endif /* __ET_RENDER_MANAGER_HPP__ */