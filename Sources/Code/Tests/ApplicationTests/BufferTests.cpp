#include "BufferTests.hpp"

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
    std::string readStr = buff.acquireString();
    ASSERT_EQ(writeStr, readStr);
}

TEST_F(BufferTests, CheckCopyBuffer) {
    char cStr[] = "Test Str";
    Buffer buff1(cStr, sizeof(cStr));
    Buffer buff2 = buff1;

    EXPECT_EQ(buff1.getReadData(), buff2.getReadData());
    EXPECT_EQ(buff1.getSize(), buff2.getSize());

    char cLongStr[] = "Long Test Str";
    Buffer buff3;
    buff3.resize(sizeof(cLongStr));
    buff3 = buff2;

    EXPECT_EQ(buff2.getReadData(), buff3.getReadData());
    EXPECT_EQ(buff2.getSize(), buff3.getSize());

    char cVeryLognStr[] = "Very Long Test Str";
    buff3.resize(sizeof(cVeryLognStr));
    memcpy(&cVeryLognStr, buff3.getWriteData(), sizeof(cVeryLognStr));

    EXPECT_EQ(sizeof(cVeryLognStr), buff3.getSize());
    std::string veryLongStr = buff3.acquireString();

    EXPECT_STREQ(veryLongStr.c_str(), cVeryLognStr);
}

TEST_F(BufferTests, CheckStringAcquire) {
    std::string str = "Test";
    Buffer buff(static_cast<const void*>(str.c_str()), str.length());

    std::string resStr = buff.acquireString();
    ASSERT_STREQ(str.c_str(), resStr.c_str());

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_STREQ(buff.getCString(), "");
}

TEST_F(BufferTests, CheckResizeToSameSize) {
    std::string str = "Test";
    Buffer buff(static_cast<const void*>(str.c_str()), str.length());

    const void* oldPtr = buff.getReadData();

    buff.resize(buff.getSize());

    const void* newPtr = buff.getReadData();

    ASSERT_EQ(oldPtr, newPtr);
}