#ifndef __VOID_TEST_RENDER_HPP__
#define __VOID_TEST_RENDER_HPP__

#include "Render/Render.hpp"

#include <mutex>

class VoidTestRender : public Render {
public:

    VoidTestRender() = default;
    virtual ~VoidTestRender() = default;

    // ETRender
    Vec2i ET_getRenderPort() const override { return Vec2i(0); }
    void ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFb) override {}
    std::shared_ptr<RenderGeometry> ET_createGeometry(const std::string& geomName) override { return nullptr; }
    std::shared_ptr<RenderMaterial> ET_createMaterial(const std::string& matName) override { return nullptr; }

protected:

    // SystemLogic
    bool onInit() override {
        bool res = false;
        {
            std::scoped_lock<std::mutex> lock(mutex);
            res = retRes_init;
        }
        return res;
    }
    void onUpdate(float dt) override {
        std::scoped_lock<std::mutex> lock(mutex);
        ++callCount_update;
    }

public:

    std::mutex mutex;
    bool retRes_init { true };
    size_t callCount_update { 0u };
};

#endif /* __VOID_TEST_RENDER_HPP__ */