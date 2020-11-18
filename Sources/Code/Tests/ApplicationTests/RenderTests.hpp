#ifndef __RENDER_TESTS_HPP__
#define __RENDER_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"
#include "Render/ImageBuffer.hpp"
#include "Render/Color.hpp"

class RenderFramebuffer;

class RenderTests : public ConsoleAppTests {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<ImageBuffer> IMAGE_BUFFER;
    static std::shared_ptr<RenderFramebuffer> RENDER_FB;

protected:

    void SetUp() override;
    void TearDown() override;

protected:

    void checkSquare(const ColorB& drawColor, size_t xStart, size_t xEnd, size_t yStart, size_t yEnd);
    void dumpImageBuffer();
};

#endif /* __RENDER_TESTS_HPP__ */