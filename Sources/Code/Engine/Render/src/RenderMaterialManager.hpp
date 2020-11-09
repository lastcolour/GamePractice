#ifndef __RENDER_MATERIAL_MANAGER_HPP__
#define __RENDER_MATERIAL_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderManager.hpp"

class RenderMaterialManager : public SystemLogic,
    public ETNode<ETRenderMaterialManager> {
public:

    RenderMaterialManager();
    virtual ~RenderMaterialManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderMaterialManager
    std::shared_ptr<RenderMaterial> ET_createMaterial(const char* matName) override;

private:

    int createProgram(const std::string& vert, const std::string& frag);
    int createProgramImpl(const std::string& vertSrc, const std::string& fragSrc);
    int createMaterialProgram(const std::string& matName);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderMaterial>> materials;
};

#endif /* __RENDER_MATERIAL_MANAGER_HPP__ */