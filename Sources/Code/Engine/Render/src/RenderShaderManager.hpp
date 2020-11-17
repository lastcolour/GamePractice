#ifndef __RENDER_SHADER_MANAGER_HPP__
#define __RENDER_SHADER_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderManager.hpp"

class RenderShaderManager : public SystemLogic,
    public ETNode<ETRenderShaderManager> {
public:

    RenderShaderManager();
    virtual ~RenderShaderManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderShaderManager
    std::shared_ptr<RenderShader> ET_createShader(const char* shaderName) override;

private:

    int createProgram(const std::string& vert, const std::string& frag);
    int createProgramImpl(const std::string& vertSrc, const std::string& fragSrc);
    int createShaderProgram(const std::string& shaderName);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderShader>> shaders;
};

#endif /* __RENDER_SHADER_MANAGER_HPP__ */