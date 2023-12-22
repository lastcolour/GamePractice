#include "MemoryStreamTests.hpp"
#include "Core/MemoryStream.hpp"

TEST_F(MemoryStreamTests, TestReadPrimitiveTypes) {
    Memory::MemoryStream stream;

    stream.openForWrite();

    ASSERT_EQ(stream.size(), 0u);
    ASSERT_EQ(stream.tellg(), 0u);
    stream.write(false);

    ASSERT_EQ(stream.size(), 1u);
    ASSERT_EQ(stream.tellg(), 1u);
    stream.write(1);

    ASSERT_EQ(stream.size(), 5u);
    ASSERT_EQ(stream.tellg(), 5u);
    stream.write("Test");

    ASSERT_EQ(stream.size(), 10u);
    ASSERT_EQ(stream.tellg(), 10u);
    stream.write(1.f);

    ASSERT_EQ(stream.size(), 14u);
    ASSERT_EQ(stream.tellg(), 14u);

    auto buffer = stream.flushToBuffer();
    stream.close();

    stream.openForRead(buffer);

    ASSERT_EQ(stream.size(), 14u);
    ASSERT_EQ(stream.tellg(), 0u);
    {
        bool boolVal = true;
        stream.read(boolVal);
        ASSERT_EQ(boolVal, false);
    }
    ASSERT_EQ(stream.size(), 14u);
    ASSERT_EQ(stream.tellg(), 1u);
    {
        int intVal = 0;
        stream.read(intVal);
        ASSERT_EQ(intVal, 1);
    }
    ASSERT_EQ(stream.size(), 14u);
    ASSERT_EQ(stream.tellg(), 5u);
    {
        std::string strVal;
        stream.read(strVal);
        ASSERT_STREQ(strVal.c_str(), "Test");
    }
    ASSERT_EQ(stream.size(), 14u);
    ASSERT_EQ(stream.tellg(), 10u);
    {
        float floatVal = 0.f;
        stream.read(floatVal);
        ASSERT_FLOAT_EQ(floatVal, 1.f);
    }
    ASSERT_EQ(stream.size(), 14u);
    ASSERT_EQ(stream.tellg(), 14u);
}