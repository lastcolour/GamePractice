#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "Platforms/OpenGL.hpp"
#include "Render/Color.hpp"

#include <unordered_map>
#include <string>
#include <memory>

class RenderMaterial;
class RenderGeometry;

class Render {
public:

    Render();
    virtual ~Render() = default;

    virtual bool init();
    virtual void update();

    void setClearColor(const ColorF& col);
    void setViewport(int width, int heigth);

    std::shared_ptr<RenderGeometry> createGeometry(const std::string& geomName);
    std::shared_ptr<RenderMaterial> createMaterial(const std::string& matName);

private:

    std::shared_ptr<RenderGeometry> createSquare();
    GLuint createProgram(const std::string& vert, const std::string& frag);
    GLuint createProgramImpl(const std::string& vertSrc, const std::string& fragSrc);

private:

    ColorF clearColor;
    std::unordered_map<std::string, std::weak_ptr<RenderMaterial>> materials;
    std::unordered_map<std::string, std::weak_ptr<RenderGeometry>> geometris;
};

#endif /* __RENDER_HPP__ */