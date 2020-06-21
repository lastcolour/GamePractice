#include "AssetsTests.hpp"
#include "Desktop/DesktopAssets.hpp"
#include "Core/JSONNode.hpp"
#include "Core/ETTimer.hpp"

#include <algorithm>

namespace {

const char* TEST_FILE_PATH = "Render/Materials.json";
const char* TEST_LOCAL_PATH = "Test.txt";

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
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, assetNameWithSlash.c_str());

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_NE(buff.getString(), "");

    assetNameWithSlash = "/";
    assetNameWithSlash += TEST_FILE_PATH;

    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, assetNameWithSlash.c_str());

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAssetWithInvalidSlashes) {
    std::string path = TEST_FILE_PATH;
    std::replace(path.begin(), path.end(), '/', '\\');

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, path.c_str());

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

    ET_SendEvent(&ETSystemTimerEvents::ET_onSystemTick, cacheLifetime);

    Buffer buff3;
    ET_SendEventReturn(buff3, &ETAssets::ET_loadAsset, TEST_FILE_PATH);

    ASSERT_TRUE(buff3);
    ASSERT_NE(buff3.getReadData(), buff2.getReadData());
}

TEST_F(AssetsTests, CheckSaveDeleteLocalFile) {
    {
        Buffer buff;
        ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, TEST_FILE_PATH);
        bool saveRes = false;
        ET_SendEventReturn(saveRes, &ETAssets::ET_saveLocalFile, TEST_LOCAL_PATH, buff);
        ASSERT_TRUE(saveRes);
    }
    {
        Buffer buff;
        ET_SendEventReturn(buff, &ETAssets::ET_loadLocalFile, TEST_LOCAL_PATH);
        ASSERT_TRUE(buff);
    }
    {
        bool removeRes = false;
        ET_SendEventReturn(removeRes, &ETAssets::ET_removeLocalFile, TEST_LOCAL_PATH);
        ASSERT_TRUE(removeRes);
    }
    {
        Buffer buff;
        ET_SendEventReturn(buff, &ETAssets::ET_loadLocalFile, TEST_LOCAL_PATH);
        ASSERT_FALSE(buff);
    }
}