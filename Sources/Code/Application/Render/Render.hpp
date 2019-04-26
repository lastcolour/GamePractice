#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/SystemLogic.hpp"
#include "Render/Camera2D.hpp"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class RenderMaterial;
class RenderGeometry;
class RenderFontSystem;

class Render : public SystemLogic,
    public ETNode<ETSurfaceEvents>,
    public ETNode<ETRender> {
public:

    Render();
    virtual ~Render();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRender
    const ColorB& ET_getClearColor() const override;
    void ET_setClearColor(const ColorB& col) override;
    void ET_drawFrame() override;
    void ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFb) override;
    Vec2i ET_getRenderPort() const override;
    const Mat4& ET_getProj2DMat4() const override;
    std::shared_ptr<RenderGeometry> ET_createGeometry(const std::string& geomName) override;
    std::shared_ptr<RenderMaterial> ET_createMaterial(const std::string& matName) override;
    std::shared_ptr<RenderFont> ET_createDefaultFont() override;

    // ETSurfaceEvents
    void ET_onSurfaceResize(const Vec2i& size) override;
    void ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) override;

private:

    void setViewport(const Vec2i& size);
    std::shared_ptr<RenderGeometry> createSquare();
    std::shared_ptr<RenderGeometry> createTextGeomChunk();
    int createProgram(const std::string& vert, const std::string& frag);
    int createProgramImpl(const std::string& vertSrc, const std::string& fragSrc);

private:

    RenderTextureFramebuffer* renderFb;
    Camera2D camera2d;
    ColorB clearColor;
    std::unique_ptr<RenderFontSystem> fontSystem;
    std::unordered_map<std::string, std::weak_ptr<RenderMaterial>> materials;
    std::unordered_map<std::string, std::weak_ptr<RenderGeometry>> geometris;
};

#endif /* __RENDER_HPP__ */