#include "RenderTests.hpp"
#include "Render/Render.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderTextureFramebuffer.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/RenderFont.hpp"
#include "Platforms/OpenGL.hpp"
#include "Entity/Entity.hpp"
#include "Math/MatrixTransform.hpp"
#include "Game/GameInitModule.hpp"
#include "Render/Logics/RenderImageLogic.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace {
    const size_t RENDER_WIDTH = 400;
    const size_t RENDER_HEIGHT = 300;

    const char* TEST_MATERIAL_1 = "geom_solid_color";
    const char* TEST_MATERIAL_2 = "geom_Solid_color";
    const char* TEST_GEOM_1 = "square";

    const char* SIMPLE_OBJECT = "Game/Simple.json";
    const float SCALE_FACTOR = 0.8f;

    const ColorB DRAW_COLOR(0, 255, 0);
    const ColorB CLEAR_COLOR(0, 0, 0);
} // namespace

std::unique_ptr<RenderTextureFramebuffer> RenderTests::textureFramebuffer;

void RenderTests::SetUpTestCase() {
    ConsoleAppTests::SetUpTestCase();

    textureFramebuffer.reset(new RenderTextureFramebuffer(RENDER_WIDTH, RENDER_HEIGHT));
    ASSERT_TRUE(textureFramebuffer->init());
    ASSERT_TRUE(textureFramebuffer->bind());
}

void RenderTests::TearDownTestCase() {
    textureFramebuffer.reset();
    ConsoleAppTests::TearDownTestCase();
}

void RenderTests::SetUp() {
    textureFramebuffer->bind();
}

void RenderTests::TearDown() {
    auto errCode = glGetError();
    ASSERT_EQ(errCode, GL_NO_ERROR);

    textureFramebuffer->clear();

    glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

void RenderTests::checkSquare(size_t xStart, size_t xEnd, size_t yStart, size_t yEnd) {

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    const Vec2i size = textureFramebuffer->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB& col = textureFramebuffer->getColor(i, j);
            if(i > xStart && i < xEnd && j > yStart && j < yEnd) {
                if(col != DRAW_COLOR) {
                    ++failPixCount;
                }
            } else if(!((i == xStart && j >= yStart && j <= yEnd)
                || (i == xEnd && j >= yStart && j <= yEnd)
                || (j == yStart && i >= xStart && i <= xEnd)
                || (j == yEnd && i >= xStart && i <= xEnd))) {

                if(col != CLEAR_COLOR) {
                    ++failPixCount;
                }
            }
        }
    }
    ASSERT_EQ(failPixCount, 0u);
}

void RenderTests::dumpFramebuffer() {
    stbi_flip_vertically_on_write(1);
    std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    testName += ".png";
    const Vec2i size = textureFramebuffer->getSize();
    const int compCount = 4;
    auto res = stbi_write_png(testName.c_str(), size.x, size.y,
        compCount, textureFramebuffer->getPtr(), compCount * size.x);

    ASSERT_NE(res, 0);
}

TEST_F(RenderTests, RestoreViewPortAfterRenderToFramebuffer) {
    textureFramebuffer->unbind();

    Vec2i origRenderPort(0);
    ET_SendEventReturn(origRenderPort, &ETRenderCamera::ET_getRenderPort);

    textureFramebuffer->bind();

    Vec2i fbRenderPort(0);
    ET_SendEventReturn(fbRenderPort, &ETRenderCamera::ET_getRenderPort);

    ASSERT_EQ(fbRenderPort, textureFramebuffer->getSize());

    textureFramebuffer->unbind();

    Vec2i resRenderPort(0);
    ET_SendEventReturn(resRenderPort, &ETRenderCamera::ET_getRenderPort);

    ASSERT_EQ(origRenderPort, resRenderPort);
}

TEST_F(RenderTests, CheckClear) {
    ColorF clearColor(1.f, 0.3f, 0.5f);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    const Vec2i size = textureFramebuffer->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB col = textureFramebuffer->getColor(i, j);
            if(col.getColorF() != clearColor) {
                ++failPixCount;
            }
        }
    }
    ASSERT_EQ(failPixCount, 0);
}

TEST_F(RenderTests, CheckEmptyRender) {
    ET_SendEvent(&ETRender::ET_setClearColor, DRAW_COLOR);
    ColorB clearCol(0, 0, 0);
    ET_SendEventReturn(clearCol, &ETRender::ET_getClearColor);
    ASSERT_EQ(DRAW_COLOR, clearCol);

    ET_SendEvent(&ETRender::ET_drawFrame);

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    const Vec2i size = textureFramebuffer->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB col = textureFramebuffer->getColor(i, j);
            if(col != DRAW_COLOR) {
                ++failPixCount;
            }
        }
    }

    ET_SendEvent(&ETRender::ET_setClearColor, CLEAR_COLOR);

    ASSERT_EQ(failPixCount, 0);
}

TEST_F(RenderTests, CheckCreateInvalidMaterial) {
    std::shared_ptr<RenderMaterial> material;
    ET_SendEventReturn(material, &ETRenderMaterialManager::ET_createMaterial, "");
    ASSERT_FALSE(material);
}

TEST_F(RenderTests, CheckCreateSameMaterial) {
    std::shared_ptr<RenderMaterial> mat1;
    ET_SendEventReturn(mat1, &ETRenderMaterialManager::ET_createMaterial, TEST_MATERIAL_1);

    std::shared_ptr<RenderMaterial> mat2;
    ET_SendEventReturn(mat2, &ETRenderMaterialManager::ET_createMaterial, TEST_MATERIAL_2);

    ASSERT_TRUE(mat1);
    ASSERT_EQ(mat1.get(), mat2.get());
}

TEST_F(RenderTests, CheckCreateSquareGeom) {
    std::shared_ptr<RenderGeometry> geom;
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, TEST_GEOM_1);

    const Vec3 size = geom->aabb.getSize();
    ASSERT_EQ(size, Vec3(2.f, 2.f, 0.f));

    const Vec3 center = geom->aabb.getCenter();
    ASSERT_EQ(center, Vec3(0.f));

    ASSERT_TRUE(geom);
}

TEST_F(RenderTests, CheckRenderSquare) {
    std::shared_ptr<RenderGeometry> geom;
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, TEST_GEOM_1);
    ASSERT_TRUE(geom);

    std::shared_ptr<RenderMaterial> material;
    ET_SendEventReturn(material, &ETRenderMaterialManager::ET_createMaterial, TEST_MATERIAL_1);
    ASSERT_TRUE(material);

    material->bind();
    material->setUniformMat4("MVP", Mat4(1.f));
    material->setUniform4f("color", DRAW_COLOR);
    geom->draw();
    material->unbind();

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    const Vec2i size = textureFramebuffer->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB col = textureFramebuffer->getColor(i, j);
            if(col != DRAW_COLOR) {
                ++failPixCount;
            }
        }
    }

    ASSERT_EQ(failPixCount, 0);
}

TEST_F(RenderTests, CheckProjectionToScreen) {
    const Vec2i fbSize = textureFramebuffer->getSize();
    const auto w = fbSize.x;
    const auto h = fbSize.y;
    const Vec3 center(w * 0.5f, h * 0.5f, 0.f);

    std::shared_ptr<RenderGeometry> geom;
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, TEST_GEOM_1);
    ASSERT_TRUE(geom);

    const Vec3 size = geom->aabb.getSize();
    const Vec3 scale = SCALE_FACTOR * Vec3(w / size.x, h / size.y, 1.f);

    Mat4 proj;
    ET_SendEventReturn(proj, &ETRenderCamera::ET_getProj2DMat4);
    Mat4 tm(1.f);
    Math::Translate(tm, center);
    Math::Scale(tm, scale);
    tm = proj * tm;

    std::shared_ptr<RenderMaterial> material;
    ET_SendEventReturn(material, &ETRenderMaterialManager::ET_createMaterial, TEST_MATERIAL_1);
    ASSERT_TRUE(material);

    material->bind();
    material->setUniformMat4("MVP", tm);
    material->setUniform4f("color", DRAW_COLOR);
    geom->draw();
    material->unbind();

    const size_t xStart = static_cast<size_t>(center.x - SCALE_FACTOR * w * 0.5f);
    const size_t xEnd = static_cast<size_t>(center.x + SCALE_FACTOR * w * 0.5f);
    const size_t yStart = static_cast<size_t>(center.y - SCALE_FACTOR * h * 0.5f);
    const size_t yEnd = static_cast<size_t>(center.y + SCALE_FACTOR * h * 0.5f);

    checkSquare(xStart, xEnd, yStart, yEnd);
    dumpFramebuffer();
}

TEST_F(RenderTests, CheckRenderOfSimpleObject) {
    EntityId objId = InvalidEntityId;
    ET_SendEventReturn(objId, &ETEntityManager::ET_createEntity, SIMPLE_OBJECT);
    ASSERT_NE(objId, InvalidEntityId);

    auto size = textureFramebuffer->getSize();
    const Vec3 center(size.x * 0.5f, size.y * 0.5f, 0.f);

    Transform tm;
    tm.pt = center;
    ET_SendEvent(objId, &ETEntity::ET_setTransform, tm);

    ET_SendEvent(objId, &ETRenderSimpleLogic::ET_setColor, DRAW_COLOR);
    ET_SendEvent(objId, &ETRenderSimpleLogic::ET_setSize, Vec2(size.x * SCALE_FACTOR, size.y * SCALE_FACTOR));

    ET_SendEvent(&ETRender::ET_drawFrame);

    const size_t xStart = static_cast<size_t>(center.x - SCALE_FACTOR * size.x * 0.5f);
    const size_t xEnd = static_cast<size_t>(center.x + SCALE_FACTOR * size.x * 0.5f);
    const size_t yStart = static_cast<size_t>(center.y - SCALE_FACTOR * size.y * 0.5f);
    const size_t yEnd = static_cast<size_t>(center.y + SCALE_FACTOR * size.y * 0.5f);

    checkSquare(xStart, xEnd, yStart, yEnd);
    dumpFramebuffer();

    ET_SendEvent(&ETEntityManager::ET_destroyEntity, objId);
}

TEST_F(RenderTests, CheckCreateSameFontTwice) {
    std::shared_ptr<RenderFont> font1;
    ET_SendEventReturn(font1, &ETRenderFontManager::ET_createDefaultFont);

    std::shared_ptr<RenderFont> font2;
    ET_SendEventReturn(font2, &ETRenderFontManager::ET_createDefaultFont);

    ASSERT_TRUE(font1);
    const Vec2i texSize = font1->getTexSize();
    ASSERT_GT(texSize, Vec2i(0));

    const int texId = font1->getTexId();
    ASSERT_NE(texId, 0);

    ASSERT_EQ(font1.get(), font2.get());
}

TEST_F(RenderTests, CheckRenderSimpleText) {
    auto gameObj = createVoidObject();
    RenderTextLogic* renderText = new RenderTextLogic;
    renderText->ET_setMaterial("text_solid_color");
    gameObj->addLogic(std::unique_ptr<EntityLogic>(renderText));
    ASSERT_TRUE(renderText->init());

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Vec2 portCenter;
    portCenter.x = renderPort.x / 2.f;
    portCenter.y = renderPort.y / 2.f;

    Transform tm;
    tm.pt = Vec3(portCenter, 0.f);
    gameObj->ET_setTransform(tm);

    renderText->ET_setText("");
    AABB2D box = renderText->ET_getTextAABB();
    ASSERT_EQ(box.getSize(), Vec2(0.f));
    ASSERT_EQ(box.getCenter(), portCenter);

    for(int ch = 32; ch < 127; ++ch)
    {
        std::string text(1, ch); 
        renderText->ET_setText(text.c_str());
        box = renderText->ET_getTextAABB();
        if (ch != '\n') {
            EXPECT_GT(box.getSize(), Vec2(0.f)) << "Char: '" << static_cast<char>(ch) << "', code: " << ch;
        }
    }

    renderText->ET_setText("Hello World!");
    ET_SendEvent(&ETRender::ET_drawFrame);

    ASSERT_TRUE(textureFramebuffer->read());

    int changedPixels = 0;
    const Vec2i size = textureFramebuffer->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB& col = textureFramebuffer->getColor(i, j);
            if(col != CLEAR_COLOR) {
                ++changedPixels;
            }
        }
    }
    dumpFramebuffer();

    ASSERT_GT(changedPixels, 0);
}

TEST_F(RenderTests, CheckRenderSimpleImage) {
    auto gameObj = createVoidObject();
    RenderImageLogic* renderImage = new RenderImageLogic;
    renderImage->ET_setMaterial("simple_image");
    gameObj->addLogic(std::unique_ptr<EntityLogic>(renderImage));
    ASSERT_TRUE(renderImage->init());

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    tm.pt = Vec3(renderPort.x / 2.f, renderPort.y / 2.f, 0.f);
    gameObj->ET_setTransform(tm);

    renderImage->ET_setImage("Images/options.png");

    Vec2i imageSize = renderImage->ET_getSize();
    ASSERT_GT(imageSize.x, 0);
    ASSERT_GT(imageSize.y, 0);

    ET_SendEvent(&ETRender::ET_drawFrame);

    ASSERT_TRUE(textureFramebuffer->read());

    int changedPixels = 0;
    const Vec2i size = textureFramebuffer->getSize();
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            const ColorB& col = textureFramebuffer->getColor(i, j);
            if(col != CLEAR_COLOR) {
                ++changedPixels;
            }
        }
    }
    dumpFramebuffer();

    ASSERT_GT(changedPixels, 0);
}