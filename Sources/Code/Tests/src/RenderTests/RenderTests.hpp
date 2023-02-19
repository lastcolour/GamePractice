#ifndef __RENDER_TESTS_HPP__
#define __RENDER_TESTS_HPP__

#include "Math/AABB.hpp"
#include "TestUtils/EngineTests.hpp"
#include "Render/ImageBuffer.hpp"
#include "Render/src/RenderFramebuffer.hpp"

class RenderTests : public EngineTests {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

    static Vec2i getRenderResolution();
    static ColorB getClearColor();
    static AABB2Di getDirtyArea();

    static size_t countNotEqualPixels(const Vec2i& start, const Vec2i& end, const ColorB& checkCol);

    static size_t countNotEqualPixels(const AABB2Di& box, const ColorB& checkCol) {
        return countNotEqualPixels(box.bot, box.top, checkCol);
    }

    static size_t countEqualPixels(const Vec2i& start, const Vec2i& end, const ColorB& checkCol);

    static size_t countEqualPixels(const AABB2Di& box, const ColorB& checkCol) {
        return countEqualPixels(box.bot, box.top, checkCol);
    }

    static void syncWithGame();
    static void syncWithGameAndDrawToImageBuffer(float dt = 0.016f);
    static void dumpImageBuffer();

protected:

    void SetUp() override;
    void TearDown() override;

protected:

    static std::unique_ptr<ImageBuffer> IMAGE_BUFFER;
    static std::shared_ptr<RenderFramebuffer> RENDER_FB;
};

#endif /* __RENDER_TESTS_HPP__ */