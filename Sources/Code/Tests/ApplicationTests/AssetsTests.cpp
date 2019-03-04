#include "AssetsTests.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {
    const char* TEST_FILE_PATH = "Render/Materials.json";
}

TEST_F(AssetsTests, CheckLoadEmptyPath) {
    Buffer buff = ET_SendEventReturn(&ETAsset::ET_loadAsset, "");

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getData(), nullptr);
    ASSERT_EQ(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAsset) {
    Buffer buff = ET_SendEventReturn(&ETAsset::ET_loadAsset, TEST_FILE_PATH);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidJSONAsset) {
    JSONNode node = ET_SendEventReturn(&ETAsset::ET_loadJSONAsset, TEST_FILE_PATH);
    ASSERT_TRUE(node);
}

TEST_F(AssetsTests, CheckLoadValidAssetWithSlashInStart) {
    std::string assetNameWithSlash = "\\";
    assetNameWithSlash += TEST_FILE_PATH;

    Buffer buff = ET_SendEventReturn(&ETAsset::ET_loadAsset, assetNameWithSlash);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");

    assetNameWithSlash = "/";
    assetNameWithSlash += TEST_FILE_PATH;

    buff = ET_SendEventReturn(&ETAsset::ET_loadAsset, assetNameWithSlash);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}

TEST_F(AssetsTests, CheckLoadValidAssetWithInvalidSlashes) {
    std::string path = TEST_FILE_PATH;
    std::replace(path.begin(), path.end(), '/', '\\');

    Buffer buff = ET_SendEventReturn(&ETAsset::ET_loadAsset, path);

    ASSERT_TRUE(buff);
    ASSERT_NE(buff.getSize(), 0u);
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_NE(buff.getString(), "");
}