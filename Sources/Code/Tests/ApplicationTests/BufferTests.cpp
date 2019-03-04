#include "BufferTests.hpp"
#include "Core/Buffer.hpp"

namespace {
    const size_t TEST_SIZE = 32;
}

TEST_F(BufferTests, CheckVoidBuffer) {
    Buffer buff;
    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckNormalBuffer) {
    Buffer buff(TEST_SIZE);
    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), TEST_SIZE);
    ASSERT_NE(buff.getData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckResize) {
    Buffer buff;
    buff.resize(TEST_SIZE);
    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), TEST_SIZE);
    ASSERT_NE(buff.getData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckWriteStr) {
    std::string testStr = "test_str ";
    Buffer buff(testStr.size());
    strcpy(static_cast<char*>(buff.getData()), testStr.c_str());

    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), testStr.size());
    ASSERT_NE(buff.getData(), nullptr);
    ASSERT_EQ(buff.getString(), testStr);
}

TEST_F(BufferTests, CheckResizeToZero) {
    Buffer buff(TEST_SIZE);
    buff.resize(0u);

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckMoveBuffer) {
    Buffer firstBuffer(TEST_SIZE);
    Buffer secondBuffer;

    secondBuffer = std::move(firstBuffer);

    ASSERT_FALSE(firstBuffer);
    ASSERT_EQ(firstBuffer.getSize(), 0u);
    ASSERT_EQ(firstBuffer.getData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(firstBuffer.getString(), voidStr);

    ASSERT_TRUE(secondBuffer);
    ASSERT_EQ(secondBuffer.getSize(), TEST_SIZE);
    ASSERT_NE(secondBuffer.getData(), nullptr);
    ASSERT_EQ(secondBuffer.getString(), voidStr);
}