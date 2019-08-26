#include "BufferTests.hpp"
#include "Core/Buffer.hpp"
#include "Core/StringFormat.hpp"

namespace {

const size_t TEST_SIZE = 32;

} // namespace

TEST_F(BufferTests, CheckVoidBuffer) {
    Buffer buff;
    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckNormalBuffer) {
    Buffer buff(TEST_SIZE);
    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), TEST_SIZE);
    ASSERT_NE(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckResize) {
    Buffer buff;
    buff.resize(TEST_SIZE);
    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), TEST_SIZE);
    ASSERT_NE(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckWriteStr) {
    std::string testStr = "test_str ";
    Buffer buff(testStr.size());
    Core::StringCopyUnsafe(static_cast<char*>(buff.getWriteData()), testStr.c_str());

    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), testStr.size());
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_EQ(buff.getString(), testStr);
}

TEST_F(BufferTests, CheckResizeToZero) {
    Buffer buff(TEST_SIZE);
    buff.resize(0u);

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckMoveBuffer) {
    Buffer firstBuffer(TEST_SIZE);
    Buffer secondBuffer;

    secondBuffer = std::move(firstBuffer);

    ASSERT_FALSE(firstBuffer);
    ASSERT_EQ(firstBuffer.getSize(), 0u);
    ASSERT_EQ(firstBuffer.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(firstBuffer.getString(), voidStr);

    ASSERT_TRUE(secondBuffer);
    ASSERT_EQ(secondBuffer.getSize(), TEST_SIZE);
    ASSERT_NE(secondBuffer.getReadData(), nullptr);
    ASSERT_EQ(secondBuffer.getString(), voidStr);
}

TEST_F(BufferTests, CheckEqualOfWriteReadData) {
    Buffer buff(TEST_SIZE);
    ASSERT_EQ(buff.getReadData(), buff.getWriteData());
}

TEST_F(BufferTests, CheckConstructWithData) {
    std::string writeStr = "Test";
    Buffer buff(static_cast<const void*>(writeStr.c_str()), writeStr.length());
    ASSERT_EQ(buff.getSize(), writeStr.length());
    std::string readStr = buff.getString();
    ASSERT_EQ(writeStr, readStr);
}