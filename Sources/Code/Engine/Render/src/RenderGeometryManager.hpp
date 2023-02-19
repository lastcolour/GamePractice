#ifndef __RENDER_GEOMETRY_MANAGER_HPP__
#define __RENDER_GEOMETRY_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
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
    std::shared_ptr<RenderGeometry> ET_createGeometry(EPrimitiveGeometryType geomType) override;

private:

    std::shared_ptr<RenderGeometry> createGeometryOfType(EPrimitiveGeometryType geomType);
    std::shared_ptr<RenderGeometry> createVec3();
    std::shared_ptr<RenderGeometry> createVec3_Tex();
    std::shared_ptr<RenderGeometry> createVec2_Tex();
    std::shared_ptr<RenderGeometry> createParticles();
    std::shared_ptr<RenderGeometry> createLine();

private:

    std::vector<std::shared_ptr<RenderGeometry>> geometries;
};

#endif /* __RENDER_GEOMETRY_MANAGER_HPP__ */