#ifndef __RENDER_GEOMETRY_MANAGER_HPP__
#define __RENDER_GEOMETRY_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderManager.hpp"

class RenderGeometryManager : public SystemLogic,
    public ETNode<ETRenderGeometryManager> {
public:

    RenderGeometryManager();
    virtual ~RenderGeometryManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderGeometryManager
    std::shared_ptr<RenderGeometry> ET_createGeometry(PrimitiveGeometryType geomType) override;

private:

    std::shared_ptr<RenderGeometry> createGeometryOfType(PrimitiveGeometryType geomType);
    std::shared_ptr<RenderGeometry> createSquare();
    std::shared_ptr<RenderGeometry> createSquareTex();
    std::shared_ptr<RenderGeometry> createTextVertexChunks();
    std::shared_ptr<RenderGeometry> createParticles();
    std::shared_ptr<RenderGeometry> createNinePatch();
    std::shared_ptr<RenderGeometry> createLine();

private:

    std::unordered_map<PrimitiveGeometryType, std::shared_ptr<RenderGeometry>> geometris;
};

#endif /* __RENDER_GEOMETRY_MANAGER_HPP__ */