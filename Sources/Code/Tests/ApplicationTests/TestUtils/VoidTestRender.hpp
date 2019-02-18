#ifndef __VOID_TEST_RENDER_HPP__
#define __VOID_TEST_RENDER_HPP__

#include "Render/Render.hpp"

#include <mutex>

class VoidTestRender : public Render {
public:

    VoidTestRender() = default;
    virtual ~VoidTestRender() = default;

    bool init() override {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_init;
        }
        return res;
    }
    void update() override {
        std::lock_guard<std::mutex> lock(mutex);
        ++callCount_update;
    }

public:

    std::mutex mutex;
    bool retRes_init { true };
    size_t callCount_update { 0u };
};

#endif /* __VOID_TEST_RENDER_HPP__ */