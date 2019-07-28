#ifndef __RENDER_GEOMETRY_MANAGER_HPP__
#define __RENDER_GEOMETRY_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class RenderGeometryManager : public SystemLogic,
    ETNode<ETRenderGeometryManager> {
public:

    RenderGeometryManager();
    virtual ~RenderGeometryManager();

    // SystemLogic
    bool init();
    void deinit();

    // ETRenderGeometryManager
    std::shared_ptr<RenderGeometry> ET_createGeometry(const char* geomName) override;

private:

    std::shared_ptr<RenderGeometry> createSquare();
    std::shared_ptr<RenderGeometry> createTextVertexChunks();

private:

    std::unordered_map<std::string, std::weak_ptr<RenderGeometry>> geometris;
};

#endif /* __RENDER_GEOMETRY_MANAGER_HPP__ */