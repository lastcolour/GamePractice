#include "BufferTests.hpp"

namespace {

const size_t TEST_SIZE = 32;

} // namespace

TEST_F(BufferTests, CheckVoidBuffer) {
    Memory::Buffer buff;
    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckNormalBuffer) {
    Memory::Buffer buff(TEST_SIZE);
    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), TEST_SIZE);
    ASSERT_NE(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckResize) {
    Memory::Buffer buff;
    buff.resize(TEST_SIZE);
    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), TEST_SIZE);
    ASSERT_NE(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckWriteStr) {
    std::string testStr = "test_str ";
    Memory::Buffer buff(testStr.size());
    Core::StringCopyUnsafe(static_cast<char*>(buff.getWriteData()), testStr.c_str());

    ASSERT_TRUE(buff);
    ASSERT_EQ(buff.getSize(), testStr.size());
    ASSERT_NE(buff.getReadData(), nullptr);
    ASSERT_EQ(buff.getString(), testStr);
}

TEST_F(BufferTests, CheckResizeToZero) {
    Memory::Buffer buff(TEST_SIZE);
    buff.resize(0u);

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_EQ(buff.getReadData(), nullptr);
    std::string voidStr = "";
    ASSERT_EQ(buff.getString(), voidStr);
}

TEST_F(BufferTests, CheckMoveBuffer) {
    Memory::Buffer firstBuffer(TEST_SIZE);
    Memory::Buffer secondBuffer;

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
    Memory::Buffer buff(TEST_SIZE);
    ASSERT_EQ(buff.getReadData(), buff.getWriteData());
}

TEST_F(BufferTests, CheckConstructWithData) {
    std::string writeStr = "Test";
    Memory::Buffer buff(static_cast<const void*>(writeStr.c_str()), writeStr.length());
    ASSERT_EQ(buff.getSize(), writeStr.length());
    std::string readStr = buff.acquireString();
    ASSERT_EQ(writeStr, readStr);
}

TEST_F(BufferTests, CheckCopyBuffer) {
    char cStr[] = "Test Str";
    Memory::Buffer buff1(cStr, sizeof(cStr));
    Memory::Buffer buff2 = buff1;

    EXPECT_EQ(buff1.getReadData(), buff2.getReadData());
    EXPECT_EQ(buff1.getSize(), buff2.getSize());

    char cLongStr[] = "Long Test Str";
    Memory::Buffer buff3;
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
    Memory::Buffer buff(static_cast<const void*>(str.c_str()), str.length());

    std::string resStr = buff.acquireString();
    ASSERT_STREQ(str.c_str(), resStr.c_str());

    ASSERT_FALSE(buff);
    ASSERT_EQ(buff.getSize(), 0u);
    ASSERT_STREQ(buff.getCString(), "");
}

TEST_F(BufferTests, CheckResizeToSameSize) {
    std::string str = "Test";
    Memory::Buffer buff(static_cast<const void*>(str.c_str()), str.length());

    const void* oldPtr = buff.getReadData();

    buff.resize(buff.getSize());

    const void* newPtr = buff.getReadData();

    ASSERT_EQ(oldPtr, newPtr);
}

TEST_F(BufferTests, CheckBigBuffer) {
    Memory::Buffer buff1;
    buff1.resize(34164);

    EXPECT_EQ(GetEnv()->GetMemoryAllocator()->getNumAliveBigChunks(), 1u);

    Memory::Buffer buff2;
    buff2 = std::move(buff1); 

    EXPECT_EQ(GetEnv()->GetMemoryAllocator()->getNumAliveBigChunks(), 1u);

    Memory::Buffer buff3(std::move(buff2));

    EXPECT_EQ(GetEnv()->GetMemoryAllocator()->getNumAliveBigChunks(), 1u);

    buff3.resize(0);

    EXPECT_EQ(GetEnv()->GetMemoryAllocator()->getNumAliveBigChunks(), 0u);
}