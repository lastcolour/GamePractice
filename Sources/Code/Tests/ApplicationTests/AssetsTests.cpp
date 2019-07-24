#include "AssetsTests.hpp"
#include "Platforms/Desktop/DesktopAssets.hpp"
#include "Core/JSONNode.hpp"
#include <Platforms/Desktop/DesktopAssets.hpp>

namespace {
    const char* TEST_FILE_PATH = "Render/Materials.json";
} // namepsace

std::unique_ptr<DesktopAssets> AssetsTests::ASSETS;

void AssetsTests::SetUpTestCase() {
    VoidAppTests::SetUpTestCase();

    ASSETS.reset(new DesktopAssets);
    ASSERT_TRUE(ASSETS->init());
}

void AssetsTests::TearDownTestCase() {
    ASSETS->deinit();
    ASSETS.reset();

    VoidAppTests::TearDownTestCase();
}

TEST_F(AssetsTests, CheckLoadEmptyPath) {
    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, "");

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getReadData(), nullptr);
    ASSERT_EQ(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAsset) {
    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, TEST_FILE_PATH);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidJSONAsset) {
    JSONNode node;
    ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, TEST_FILE_PATH);
    ASSERT_TRUE(node);
}

TEST_F(AssetsTests, CheckLoadValidAssetWithSlashInStart) {
    std::string assetNameWithSlash = "\\";
    assetNameWithSlash += TEST_FILE_PATH;

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, assetNameWithSlash);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_NE(buff.getString(), "");

    assetNameWithSlash = "/";
    assetNameWithSlash += TEST_FILE_PATH;

    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, assetNameWithSlash);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAssetWithInvalidSlashes) {
    std::string path = TEST_FILE_PATH;
    std::replace(path.begin(), path.end(), '/', '\\');

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, path);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckAssetsCache) {
    ASSERT_TRUE(ET_IsExistNode<ETAssetsCacheManager>());

    Buffer buff1;
    ET_SendEventReturn(buff1, &ETAssets::ET_loadAsset, TEST_FILE_PATH);

    Buffer buff2;
    ET_SendEventReturn(buff2, &ETAssets::ET_loadAsset, TEST_FILE_PATH);

    ASSERT_TRUE(buff1);
    ASSERT_EQ(buff1.getReadData(), buff2.getReadData());

    float cacheLifetime = 0.f;
    ET_SendEventReturn(cacheLifetime, &ETAssetsCacheManager::ET_getCacheLifetime);
    cacheLifetime += 1.f;

    ET_SendEvent(&ETTimerEvents::ET_onTick, cacheLifetime);

    Buffer buff3;
    ET_SendEventReturn(buff3, &ETAssets::ET_loadAsset, TEST_FILE_PATH);

    ASSERT_TRUE(buff3);
    ASSERT_NE(buff3.getReadData(), buff2.getReadData());
}