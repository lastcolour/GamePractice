#include "MemoryStreamTests.hpp"
#include <Core/MemoryStream.hpp>

TEST_F(MemoryStreamTests, TestReadPrimitiveTypes) {
    MemoryStream stream;

    stream.openForWrite();

    ASSERT_EQ(stream.size(), 0);
    ASSERT_EQ(stream.tellg(), 0);
    stream.write(false);

    ASSERT_EQ(stream.size(), 1);
    ASSERT_EQ(stream.tellg(), 1);
    stream.write(1);

    ASSERT_EQ(stream.size(), 5);
    ASSERT_EQ(stream.tellg(), 5);
    stream.write("Test");

    ASSERT_EQ(stream.size(), 10);
    ASSERT_EQ(stream.tellg(), 10);
    stream.write(1.f);

    ASSERT_EQ(stream.size(), 14);
    ASSERT_EQ(stream.tellg(), 14);

    auto buffer = stream.flushToBuffer();
    stream.close();

    stream.openForRead(buffer);

    ASSERT_EQ(stream.size(), 14);
    ASSERT_EQ(stream.tellg(), 0);
    {
        bool boolVal = true;
        stream.read(boolVal);
        ASSERT_EQ(boolVal, false);
    }
    ASSERT_EQ(stream.size(), 14);
    ASSERT_EQ(stream.tellg(), 1);
    {
        int intVal = 0;
        stream.read(intVal);
        ASSERT_EQ(intVal, 1);
    }
    ASSERT_EQ(stream.size(), 14);
    ASSERT_EQ(stream.tellg(), 5);
    {
        std::string strVal;
        stream.read(strVal);
        ASSERT_STREQ(strVal.c_str(), "Test");
    }
    ASSERT_EQ(stream.size(), 14);
    ASSERT_EQ(stream.tellg(), 10);
    {
        float floatVal = 0.f;
        stream.read(floatVal);
        ASSERT_FLOAT_EQ(floatVal, 1.f);
    }
    ASSERT_EQ(stream.size(), 14);
    ASSERT_EQ(stream.tellg(), 14);
}