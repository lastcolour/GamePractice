#include "RenderTests/RenderTests.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETTasks.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Render/ETRenderTickManager.hpp"
#include "Render/src/RenderUtils.hpp"
#include "Render/src/Logics/RenderSimpleLogic.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

std::unique_ptr<ImageBuffer> RenderTests::IMAGE_BUFFER;
std::shared_ptr<RenderFramebuffer> RenderTests::RENDER_FB;

void RenderTests::SetUpTestCase() {
    EngineTests::CreateTestApp(ETestAppModules::CheckRender);

    IMAGE_BUFFER.reset(new ImageBuffer());
    IMAGE_BUFFER->setSizeAndClear(getRenderResolution());

    ET_SendEventReturn(RENDER_FB, &ETRenderTextureManager::ET_createFramebuffer, EFramebufferType::Color, IMAGE_BUFFER->getSize());
    ASSERT_TRUE(RENDER_FB);

    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, IMAGE_BUFFER->getSize());
}

void RenderTests::TearDownTestCase() {
    IMAGE_BUFFER.reset();
    RENDER_FB.reset();
    EngineTests::TearDownTestCase();
}

Vec2i RenderTests::getRenderResolution() {
    return Vec2i(400, 400);
}

ColorB RenderTests::getClearColor() {
    return ColorB(0, 0, 0);
}

AABB2Di RenderTests::getDirtyArea() {
    bool isDirty = false;
    AABB2Di dirtyBox(IMAGE_BUFFER->getSize(), Vec2i(0));

    const Vec2i size = IMAGE_BUFFER->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB& col = IMAGE_BUFFER->getColor(i, j);
            if(col != getClearColor()) {
                dirtyBox.bot.x = std::min(dirtyBox.bot.x, i);
                dirtyBox.bot.y = std::min(dirtyBox.bot.y, j);
                dirtyBox.top.x = std::max(dirtyBox.top.x, i);
                dirtyBox.top.y = std::max(dirtyBox.top.y, j);
                isDirty = true;
            }
        }
    }

    if(!isDirty) {
        return AABB2Di(Vec2i(0), Vec2i(0));
    }

    return dirtyBox;
}

void RenderTests::syncWithGame() {
    ET_SendEvent(&ETRenderUpdateTask::ET_PreRender);

    if(auto errStr = RenderUtils::GetGLError()) {
        FAIL() << "[RenderTests::syncWithGame] Error during sync with game: " << errStr;
        return;
    }
}

void RenderTests::syncWithGameAndDrawToImageBuffer(float dt) {
    ET_SendEvent(&ETRenderUpdateTask::ET_PreRender);

    if(auto errStr = RenderUtils::GetGLError()) {
        FAIL() << "[RenderTests::syncWithGameAndDrawToImageBuffer] Error during sync with game: " << errStr;
        return;
    }

    ET_SendEvent(&ETRenderTickManager::ET_onRenderTick, dt);

    ET_SendEvent(&ETRender::ET_drawFrameToBuffer, *IMAGE_BUFFER, IMAGE_BUFFER->getSize(),
        EDrawContentFilter::NoDebugInfo);
}

size_t RenderTests::countNotEqualPixels(const Vec2i& start, const Vec2i& end, const ColorB& checkCol) {
    size_t failPixCount = 0;
    const Vec2i size = IMAGE_BUFFER->getSize();
    for(int x = 0; x < size.x; ++x) {
        for(int y = 0; y < size.y; ++y) {
            const ColorB& col = IMAGE_BUFFER->getColor(x, y);
            if(x > start.x && x < end.x && y > start.y && y < end.y) {
                if(checkCol != col) {
                    ++failPixCount;
                }
            }
        }
    }
    return failPixCount;
}

size_t RenderTests::countEqualPixels(const Vec2i& start, const Vec2i& end, const ColorB& checkCol) {
    size_t equalPixelCount = 0;
    const Vec2i size = IMAGE_BUFFER->getSize();
    for(int x = 0; x < size.x; ++x) {
        for(int y = 0; y < size.y; ++y) {
            const ColorB& col = IMAGE_BUFFER->getColor(x, y);
            if(x > start.x && x < end.x && y > start.y && y < end.y) {
                if(checkCol == col) {
                    ++equalPixelCount;
                }
            }
        }
    }
    return equalPixelCount;
}

void RenderTests::dumpImageBuffer() {
    stbi_flip_vertically_on_write(1);
    std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    testName += ".png";
    const Vec2i size = IMAGE_BUFFER->getSize();
    const int compCount = 4;
    auto res = stbi_write_png(testName.c_str(), size.x, size.y,
        compCount, IMAGE_BUFFER->getData().getReadData(), compCount * size.x);

    ASSERT_NE(res, 0);
}

void RenderTests::SetUp() {
    EngineTests::SetUp();

    IMAGE_BUFFER->clear();

    auto clearColor = getClearColor();

    RENDER_FB->bind();
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    RENDER_FB->unbind();
}

void RenderTests::TearDown() {
    EngineTests::TearDown();

    auto errCode = glGetError();
    ASSERT_EQ(errCode, static_cast<GLenum>(GL_NO_ERROR));

    auto renderNodes = ET_GetAll<ETRenderNode>();
    ASSERT_TRUE(renderNodes.empty());

    auto fbSize = RENDER_FB->color0.getSize();
    auto imageSize = IMAGE_BUFFER->getSize();
    ASSERT_EQ(fbSize.x, imageSize.x);
    ASSERT_EQ(fbSize.y, imageSize.y);
}

TEST_F(RenderTests, CheckCreateInvalidShader) {
    std::shared_ptr<RenderShader> shader;
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "");
    ASSERT_FALSE(shader);
}

TEST_F(RenderTests, CheckCreateGeo) {
    {
        std::shared_ptr<RenderGeometry> geom;
        ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Line);
        ASSERT_TRUE(geom);
    }
    {
        std::shared_ptr<RenderGeometry> geom;
        ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Particles);
        ASSERT_TRUE(geom);
    }
    {
        std::shared_ptr<RenderGeometry> geom;
        ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec2_Tex);
        ASSERT_TRUE(geom);
    }
    {
        std::shared_ptr<RenderGeometry> geom;
        ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3);
        ASSERT_TRUE(geom);
    }
    {
        std::shared_ptr<RenderGeometry> geom;
        ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3_Tex);
        ASSERT_TRUE(geom);
    }
}

TEST_F(RenderTests, CheckRenderWithMask) {
    Vec2i renderRes = getRenderResolution();

    auto firstEnt = createVoidObject();
    {
        auto logicPtr = firstEnt->addCustomLogic<RenderSimpleLogic>();
        ASSERT_TRUE(logicPtr);

        auto halfSize = Vec2(renderRes.x / 2.f, renderRes.y / 2.f);
        logicPtr->ET_setSize(halfSize);
        logicPtr->ET_setColor(ColorB(255, 0, 0));
        logicPtr->ET_setZIndex(0);

        StencilWirteReadData stencilData;
        stencilData.opType = EStencilOpType::Write;
        stencilData.refVal = 1;
        logicPtr->ET_setStencilData(stencilData);

        Transform tm;
        tm.pt = Vec3(renderRes.x / 2.f, renderRes.y / 2.f, 0.f);
        firstEnt->ET_setTransform(tm);
    }

    Vec2 ptShift = Vec2(renderRes.x / 4.f, 0.f);
    auto secondEnt = createVoidObject();
    {
        auto logicPtr = secondEnt->addCustomLogic<RenderSimpleLogic>();
        ASSERT_TRUE(logicPtr);

        auto halfSize = Vec2(renderRes.x / 2.f, renderRes.y / 2.f);
        logicPtr->ET_setSize(halfSize);
        logicPtr->ET_setColor(ColorB(0, 255, 0));
        logicPtr->ET_setZIndex(1);

        StencilWirteReadData stencilData;
        stencilData.opType = EStencilOpType::ReadIncrease;
        stencilData.refVal = 1;
        logicPtr->ET_setStencilData(stencilData);

        Transform tm;
        tm.pt = Vec3(renderRes.x / 2.f, renderRes.y / 2.f, 0.f);
        tm.pt += Vec3(ptShift, 0.f);
        secondEnt->ET_setTransform(tm);
    }

    ptShift = Vec2(renderRes.x / 4.f, renderRes.y / 4.f);
    auto thirdEnt = createVoidObject();
    {
        auto logicPtr = thirdEnt->addCustomLogic<RenderSimpleLogic>();
        ASSERT_TRUE(logicPtr);

        auto halfSize = Vec2(renderRes.x / 2.f, renderRes.y / 2.f);
        logicPtr->ET_setSize(halfSize);
        logicPtr->ET_setColor(ColorB(0, 0, 255));
        logicPtr->ET_setZIndex(2);

        StencilWirteReadData stencilData;
        stencilData.opType = EStencilOpType::Read;
        stencilData.refVal = 2;
        logicPtr->ET_setStencilData(stencilData);

        Transform tm;
        tm.pt = Vec3(renderRes.x / 2.f, renderRes.y / 2.f, 0.f);
        tm.pt += Vec3(ptShift, 0.f);
        thirdEnt->ET_setTransform(tm);
    }

    syncWithGameAndDrawToImageBuffer();

    Vec2i c = renderRes / 2;

    EXPECT_EQ(0, countNotEqualPixels(Vec2i(c.x - c.x / 2, c.y - c.y / 2), Vec2i(c.x, c.y + c.y / 2), ColorB{255, 0, 0}));
    EXPECT_EQ(0, countNotEqualPixels(Vec2i(c.x, c.y - c.y  / 2), Vec2i(c.x + c.x / 2, c.y), ColorB{0, 255, 0}));
    EXPECT_EQ(0, countNotEqualPixels(Vec2i(c.x, c.y), Vec2i(c.x + c.x / 2, c.y + c.y / 2), ColorB{0, 0, 255}));
}

TEST_F(RenderTests, CheckHideUnhide) {
    const Vec2i renderRes = getRenderResolution();
    const Vec3 center(renderRes.x / 2.f, renderRes.y / 2.f, 0.f);
    const ColorB drawColor = ColorB(255, 0, 0);

    auto box = createVoidObject();
    {
        auto logicPtr = box->addCustomLogic<RenderSimpleLogic>();
        ASSERT_TRUE(logicPtr);

        logicPtr->ET_setSize(Vec2(static_cast<float>(renderRes.x), static_cast<float>(renderRes.y)));
        logicPtr->ET_setColor(drawColor);
        logicPtr->ET_setZIndex(0);

        Transform tm;
        tm.pt = Vec3(renderRes.x / 2.f, renderRes.y / 2.f, 0.f);
        box->ET_setTransform(tm);
    }

    {
        syncWithGameAndDrawToImageBuffer();
        EXPECT_EQ(0, countNotEqualPixels(Vec2i{0}, renderRes, drawColor));
    }

    ET_SendEvent(box->getEntityId(), &ETRenderNode::ET_hide);

    {
        syncWithGameAndDrawToImageBuffer();
        EXPECT_EQ(0, countNotEqualPixels(Vec2i{0}, renderRes, getClearColor()));
    }

    ET_SendEvent(box->getEntityId(), &ETRenderNode::ET_show);

    {
        syncWithGameAndDrawToImageBuffer();
        EXPECT_EQ(0, countNotEqualPixels(Vec2i{0}, renderRes, drawColor));
    }
}

TEST_F(RenderTests, CheckRenderPriority) {
    const auto renderRes = getRenderResolution();
    const Vec2 center(renderRes.x * 0.5f, renderRes.y * 0.5f);
    const Vec2 drawSize = Vec2(renderRes.x * 0.8f, renderRes.y * 0.8f);
    const ColorB drawColorA(0, 255, 0);
    const ColorB drawColorB(128, 128, 0);

    auto firstEnt = createVoidObject();
    {
        auto logicPtr = firstEnt->addCustomLogic<RenderSimpleLogic>();
        ASSERT_TRUE(logicPtr);

        logicPtr->ET_setSize(drawSize);
        logicPtr->ET_setColor(drawColorA);
        logicPtr->ET_setZIndex(0);

        Transform tm;
        tm.pt = Vec3(center, 0.f);
        firstEnt->ET_setTransform(tm);
    }

    auto secondEnt = createVoidObject();
    {
        auto logicPtr = secondEnt->addCustomLogic<RenderSimpleLogic>();
        ASSERT_TRUE(logicPtr);

        logicPtr->ET_setSize(drawSize);
        logicPtr->ET_setColor(drawColorB);
        logicPtr->ET_setZIndex(0);

        Transform tm;
        tm.pt = Vec3(center, 0.f);
        secondEnt->ET_setTransform(tm);
    }

    ET_SendEvent(firstEnt->getEntityId(), &ETRenderNode::ET_setZIndex, 1);
    ET_SendEvent(secondEnt->getEntityId(), &ETRenderNode::ET_setZIndex, 0);

    syncWithGameAndDrawToImageBuffer();

    AABB2Di checkBox;
    checkBox.setCenterAndSize(Vec2i(static_cast<int>(center.x), static_cast<int>(center.y)),
        Vec2i(static_cast<int>(drawSize.x), static_cast<int>(drawSize.y)));

    EXPECT_EQ(0, countNotEqualPixels(checkBox, drawColorA));

    ET_SendEvent(firstEnt->getEntityId(), &ETRenderNode::ET_setZIndex, 0);
    ET_SendEvent(secondEnt->getEntityId(), &ETRenderNode::ET_setZIndex, 1);

    syncWithGameAndDrawToImageBuffer();

    EXPECT_EQ(0, countNotEqualPixels(checkBox, drawColorB));
}

TEST_F(RenderTests, CheckCreateSameEmptyTexture) {
    Vec2i texSize(100);
    std::shared_ptr<RenderTexture> tex1;
    ET_SendEventReturn(tex1, &ETRenderTextureManager::ET_createTexture, ETextureDataType::R8, Vec2i(1));
    EXPECT_TRUE(tex1);

    std::shared_ptr<RenderTexture> tex2;
    ET_SendEventReturn(tex2, &ETRenderTextureManager::ET_createTexture, ETextureDataType::R8, Vec2i(1));
    EXPECT_TRUE(tex2);

    EXPECT_NE(tex1.get(), tex2.get());
}