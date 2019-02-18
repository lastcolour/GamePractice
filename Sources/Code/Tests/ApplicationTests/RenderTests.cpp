#include "RenderTests.hpp"
#include "Render/Render.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderTextureFramebuffer.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Render/RenderLogic.hpp"
#include "JSONNode.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace {
    const GLsizei RENDER_WIDTH = 128;
    const GLsizei RENDER_HEIGHT = 128;

    const char* TEST_MATERIAL_1 = "solid_color";
    const char* TEST_MATERIAL_2 = "Solid_color";
    const char* TEST_GEOM_1 = "square";
    const char* TEST_GEOM_2 = "Square";
}

std::unique_ptr<VoidTestApplication> RenderTests::app;
std::unique_ptr<RenderTextureFramebuffer> RenderTests::textureFramebuffer;

void RenderTests::SetUpTestCase() {
    app.reset(new VoidTestApplication(new DesktopPlatform(0, nullptr)));
    app->retRes_createModuleFactory.reset(new OnlyVoidGameModuleFactory);
    ASSERT_TRUE(app->init());
    ASSERT_TRUE(GetEnv()->getRender());

    textureFramebuffer.reset(new RenderTextureFramebuffer(RENDER_WIDTH, RENDER_HEIGHT));
    ASSERT_TRUE(textureFramebuffer->init());
    ASSERT_TRUE(textureFramebuffer->bind());
}

void RenderTests::TearDownTestCase() {
    textureFramebuffer.reset();
    app.reset();
}

void RenderTests::SetUp() {
}

void RenderTests::TearDown() {
    auto errCode = glGetError();
    ASSERT_EQ(errCode, GL_NO_ERROR);

    std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    testName += ".png";
    auto res = stbi_write_png(testName.c_str(), textureFramebuffer->getWidth(), textureFramebuffer->getHeight(),
        textureFramebuffer->getCompCount(), textureFramebuffer->getPtr(),
            textureFramebuffer->getCompCount() * textureFramebuffer->getWidth());

    ASSERT_NE(res, 0);

    textureFramebuffer->clear();

    glClearColor(0.f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

TEST_F(RenderTests, CheckClear) {
    ColorF clearColor(0.f, 0.0f, 0.0f);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    for(GLsizei i=0; i<textureFramebuffer->getWidth(); ++i) {
        for(GLsizei j=0; j<textureFramebuffer->getHeight(); ++j) {
            const ColorF col = textureFramebuffer->getColor(i, j);
            if(col != clearColor) {
                ++failPixCount;
            }
        }
    }
    ASSERT_EQ(failPixCount, 0);
}

TEST_F(RenderTests, CheckEmptyRender) {
    auto render = GetEnv()->getRender();
    ColorF clearColor(0.f, 1.f, 0.f);
    render->setClearColor(clearColor);

    render->update();

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    for(GLsizei i=0; i<textureFramebuffer->getWidth(); ++i) {
        for(GLsizei j=0; j<textureFramebuffer->getHeight(); ++j) {
            const ColorF& col = textureFramebuffer->getColor(i, j);
            if(col != clearColor) {
                ++failPixCount;
            }
        }
    }
    ASSERT_EQ(failPixCount, 0);
}

TEST_F(RenderTests, CheckCreateMaterial) {
    auto render = GetEnv()->getRender();
    auto material = render->createMaterial(TEST_MATERIAL_1);
    ASSERT_TRUE(material);
}

TEST_F(RenderTests, CheckCreateInvalidMaterial) {
    auto render = GetEnv()->getRender();
    auto material = render->createMaterial("");
    ASSERT_FALSE(material);
}

TEST_F(RenderTests, CheckCreateSameMaterial) {
    auto render = GetEnv()->getRender();
    auto mat1 = render->createMaterial(TEST_MATERIAL_1);
    auto mat2 = render->createMaterial(TEST_MATERIAL_2);
    ASSERT_EQ(mat1.get(), mat2.get());
}

TEST_F(RenderTests, CheckCreateSquareGeom) {
    auto render = GetEnv()->getRender();
    auto geom = render->createGeometry(TEST_GEOM_1);
    ASSERT_TRUE(geom);
}

TEST_F(RenderTests, CheckCreateSameSquareGeom) {
    auto render = GetEnv()->getRender();
    auto geom1 = render->createGeometry(TEST_GEOM_1);
    auto geom2 = render->createGeometry(TEST_GEOM_2);
    ASSERT_EQ(geom1.get(), geom2.get());
}

TEST_F(RenderTests, CheckRenderSquare) {
    auto render = GetEnv()->getRender();

    auto geom = render->createGeometry(TEST_GEOM_1);
    auto material = render->createMaterial(TEST_MATERIAL_1);

    ColorF drawColor(0.5f, 0.5f, 0.5f);
    material->bind();
    material->setUniform("Color", drawColor);
    geom->draw();
    material->unbind();

    ASSERT_TRUE(textureFramebuffer->read());

    size_t failPixCount = 0;
    for(GLsizei i=0; i<textureFramebuffer->getWidth(); ++i) {
        for(GLsizei j=0; j<textureFramebuffer->getHeight(); ++j) {
            const ColorF& col = textureFramebuffer->getColor(i, j);
            if(col != drawColor) {
                ++failPixCount;
            }
        }
    }
    ASSERT_EQ(failPixCount, 0);
}

TEST_F(RenderTests, CheckRenderOfRenderLogic) {
    std::string renderLogicData = StringFormat("{\"mat\":\"%s\", \"geom\":\"%s\"}", TEST_MATERIAL_1, TEST_GEOM_1);
    auto nodeData = JSONNode::ParseString(renderLogicData);

    ASSERT_TRUE(nodeData);
    RenderLogic logic;
    ASSERT_TRUE(logic.init(nodeData));

    auto render = GetEnv()->getRender();
    render->update();

    ColorF drawColor(0.5f, 0.5f, 0.5f);
    size_t failPixCount = 0;
    for(GLsizei i=0; i<textureFramebuffer->getWidth(); ++i) {
        for(GLsizei j=0; j<textureFramebuffer->getHeight(); ++j) {
            const ColorF& col = textureFramebuffer->getColor(i, j);
            if(col != drawColor) {
                ++failPixCount;
            }
        }
    }

    ASSERT_EQ(failPixCount, 0);
}