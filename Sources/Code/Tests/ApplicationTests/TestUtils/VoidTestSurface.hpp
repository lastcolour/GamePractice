#ifndef __VOID_TEST_SURFACE_HPP__
#define __VOID_TEST_SURFACE_HPP__

#include "Surface.hpp"

#include <mutex>

class VoidTestSurface : public Surface {
public:

    VoidTestSurface() = default;
    virtual ~VoidTestSurface() = default;

    bool init() override { 
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_init;
        }
        return res;
    }
    bool show() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_show;
        }
        return res;
    }
    bool hide() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_hide;
        }
        return res;
    }
    bool shouldRun() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_shouldRun;
        }
        return res;
    }
    int getWidth() const override {
        int res = 0;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_getWidth;
        }
        return res;
    }
    int getHeight() const {
        int res = 0;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_getWidth;
        }
        return res;
    }
    void update() override {
        std::lock_guard<std::mutex> lock(mutex);
        ++callCount_update;
    }
    void terminate() override { }
    void swapBuffers() override { }

public:

    mutable std::mutex mutex;
    bool retRes_init { true };
    bool retRes_show { true };
    bool retRes_hide { true };
    bool retRes_shouldRun { true };
    int retRes_getWidth { 600 };
    int retRes_getHeight { 480 };
    size_t callCount_update { 0 };
};

#endif /* __VOID_TEST_SURFACE_HPP__ */