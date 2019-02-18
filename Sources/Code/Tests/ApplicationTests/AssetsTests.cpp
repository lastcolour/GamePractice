#include "AssetsTests.hpp"
#include "Environment.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Assets.hpp"

namespace {
    const char* TEST_FILE_PATH = "Render/Materials.json";
}

std::unique_ptr<Application> AssetsTests::APP;

void AssetsTests::SetUpTestCase() {
    VoidTestApplication* testApp = new VoidTestApplication(new DesktopPlatform(0, nullptr));
    testApp->retRes_createModuleFactory.reset(new ConsoleAppModuleFactory);
    ASSERT_TRUE(testApp->init());
    ASSERT_TRUE(GetEnv()->getAssets());
    APP.reset(testApp);
}

void AssetsTests::TearDownTestCase() {
    APP.reset();
}

TEST_F(AssetsTests, CheckLoadEmptyPath) {
    Buffer buff = GetEnv()->getAssets()->loadAsset("");

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getData(), nullptr);
    ASSERT_EQ(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAsset) {
    Buffer buff = GetEnv()->getAssets()->loadAsset(TEST_FILE_PATH);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAssetWithSlashInStart) {
    std::string assetNameWithSlash = "\\";
    assetNameWithSlash += TEST_FILE_PATH;

    Buffer buff = GetEnv()->getAssets()->loadAsset(assetNameWithSlash);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");

    assetNameWithSlash = "/";
    assetNameWithSlash += TEST_FILE_PATH;

    buff = GetEnv()->getAssets()->loadAsset(assetNameWithSlash);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAssetWithInvalidSlashes) {
    std::string path = TEST_FILE_PATH;
    std::replace(path.begin(), path.end(), '/', '\\');

    Buffer buff = GetEnv()->getAssets()->loadAsset(path);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}