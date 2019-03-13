#ifndef __VOID_TEST_SURFACE_HPP__
#define __VOID_TEST_SURFACE_HPP__

#include "Platform.hpp"

#include <mutex>

class VoidTestSurface : public Surface {
public:

    VoidTestSurface() : retRes_size(640, 480) {}
    virtual ~VoidTestSurface() = default;

    // ETSurface
    bool ET_show() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_show;
        }
        return res;
    }
    bool ET_hide() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_hide;
        }
        return res;
    }
    Vec2i ET_getSize() const override {
        Vec2i res(0);
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_size;
        }
        return res;
    }
    void ET_terminate() override { }
    void ET_swapBuffers() override { }
    bool ET_isVisible() const override { return false; }
    GLContextType ET_getGLContextType() const override { return GLContextType::None; }

protected:

    // SystemLogic
    bool onInit() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_init;
        }
        return res;
    }
    bool onShouldRun() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_shouldRun;
        }
        return res;
    }
    void onUpdate(float dt) override {
        std::lock_guard<std::mutex> lock(mutex);
        ++callCount_update;
    }

public:

    mutable std::mutex mutex;
    bool retRes_init { true };
    bool retRes_show { true };
    bool retRes_hide { true };
    bool retRes_shouldRun { true };
    Vec2i retRes_size;
    size_t callCount_update { 0 };
};

#endif /* __VOID_TEST_SURFACE_HPP__ */