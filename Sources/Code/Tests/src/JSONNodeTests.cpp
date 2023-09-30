#include "JSONNodeTests.hpp"
#include "Core/JSONNode.hpp"

#include <map>

TEST_F(JSONNodeTests, ParseEmptyBuffer) {
    Memory::Buffer buff;
    auto node = JSONNode::ParseBuffer(buff);
    ASSERT_FALSE(node);
}

TEST_F(JSONNodeTests, ParseEmptyString) {
    auto node = JSONNode::ParseString("");
    ASSERT_FALSE(node);
}

TEST_F(JSONNodeTests, ParseStringWithEmptyObject) {
    auto node = JSONNode::ParseString("{}");
    ASSERT_TRUE(node);
}

TEST_F(JSONNodeTests, CheckObjectSize) {
    auto node = JSONNode::ParseString("{\"obj\":{}}");
    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 1);
}

TEST_F(JSONNodeTests, CheckSimpleObject) {
    auto node = JSONNode::ParseString("{\"int\":1, \"float\":1.0, \"str\":\"1\"}");
    ASSERT_TRUE(node);

    int intVal = 0;
    node.read("int", intVal);
    ASSERT_EQ(intVal, 1);

    float floatVal = 0;
    node.read("float", floatVal);
    ASSERT_EQ(floatVal, 1.f);

    std::string strVal;
    node.read("str", strVal);
    ASSERT_STREQ(strVal.c_str(), "1");
}

TEST_F(JSONNodeTests, CheckBeginEnd) {
    auto node = JSONNode::ParseString("{\"obj1\": 1}");

    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 1);

    auto begIt = node.begin();
    auto endIt = node.end();

    ASSERT_NE(begIt, endIt);
    ++begIt;
    ASSERT_EQ(begIt, endIt);
}

TEST_F(JSONNodeTests, CheckNodeAccessThroughtIterator) {
    auto node = JSONNode::ParseString("{\"obj1\": 1}");
    auto begIt = node.begin();

    std::string nodeKey = begIt->key();
    ASSERT_STREQ(nodeKey.c_str(), "obj1");

    int intVal = 0;
    begIt->read(intVal);
    ASSERT_EQ(intVal, 1);
}

TEST_F(JSONNodeTests, CheckForLoop) {
    auto node = JSONNode::ParseString("{\"obj1\": 1, \"obj2\": 2}");

    std::map<std::string, int> keyVal;
    size_t totalIteration = 0;
    int intVal = 0;
    for(const auto& item : node) {
        item.read(intVal);
        keyVal[item.key()] = intVal;
        ++totalIteration;
    }

    ASSERT_EQ(totalIteration, 2u);
    ASSERT_EQ(keyVal.size(), 2u);
    ASSERT_EQ(keyVal.count("obj1"), 1u);
    ASSERT_EQ(keyVal.count("obj2"), 1u);
    ASSERT_EQ(keyVal["obj1"], 1);
    ASSERT_EQ(keyVal["obj2"], 2);
}

TEST_F(JSONNodeTests, CheckForLoopInArray) {
    auto node = JSONNode::ParseString("{\"arr\":[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]}");
    auto arr = node.object("arr");

    ASSERT_TRUE(arr);
    ASSERT_STREQ(arr.key(), "arr");

    std::vector<int> intVals;
    for(const auto& elem : arr) {
        int intVal = 0;
        elem.read(intVal);
        intVals.push_back(intVal);
    }

    ASSERT_EQ(intVals.size(), 10u);
    for(size_t i=0u,sz=intVals.size(); i<sz; ++i) {
        ASSERT_EQ(intVals[i], static_cast<int>(i+1));
    }
}

TEST_F(JSONNodeTests, CheckAccessToInvalidElements) {
    auto node = JSONNode::ParseString("{\"obj\": {}}");

    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 1);

    auto objNode = node.object("invalid");
    ASSERT_FALSE(objNode);

    std::string str = "empty_val";
    objNode.read("invalid", str);
    ASSERT_STREQ(str.c_str(), "empty_val");
    objNode.read(str);
    ASSERT_STREQ(str.c_str(), "empty_val");

    float floatVal = -1.f;
    objNode.read("invalid", floatVal);
    ASSERT_EQ(floatVal, -1.f);
    objNode.read(floatVal);
    ASSERT_EQ(floatVal, -1.f);

    int intVal = -1;
    objNode.read("invalid", intVal);
    ASSERT_EQ(intVal, -1);
    objNode.read(intVal);
    ASSERT_EQ(intVal, -1);

    size_t iterCount = 0;
    for(const auto& elem : objNode) {
        (void)elem;
        ++iterCount;
    }

    ASSERT_EQ(iterCount, 0);
    ASSERT_EQ(objNode.begin(), objNode.end());
}

TEST_F(JSONNodeTests, CheckWriteSimpleValues) {
    JSONNode node;
    {
        bool writeVal = false;
        node.write("bool_value", writeVal);

        bool readVal = true;
        node.read("bool_value", readVal);

        EXPECT_EQ(writeVal, readVal);
    }

    {
        int writeVal = 1;
        node.write("int_value", writeVal);

        int readVal = 0;
        node.read("int_value", readVal);

        EXPECT_EQ(writeVal, readVal);
    }

    {
        float writeVal = -1.f;
        node.write("float_value", writeVal);

        float readVal = 0.f;
        node.read("float_value", readVal);

        EXPECT_FLOAT_EQ(writeVal, readVal);
    }

    {
        std::string writeVal = "test";
        node.write("string_value", writeVal);

        std::string readVal = "";
        node.read("string_value", readVal);

        EXPECT_EQ(writeVal, readVal);
    }
}

TEST_F(JSONNodeTests, CheckWriteObject) {
    JSONNode rootNode;
    int writeVal = 1;
    {
        JSONNode childNode;
        childNode.write("val", writeVal);
        rootNode.write("obj", childNode);
    }

    JSONNode childNode = rootNode.object("obj");
    EXPECT_TRUE(childNode);

    int readVal = 0;
    childNode.read("val", readVal);
    EXPECT_EQ(readVal, writeVal);
}

TEST_F(JSONNodeTests, CheckCreateReadSame) {
    Memory::Buffer buff;
    int write_val = 1;
    {
        JSONNode node;
        node.write("val", write_val);
        buff = node.flushToBuffer();
        ASSERT_TRUE(buff);
    }
    int read_val = 0;
    {
        std::string str = buff.acquireString();
        auto node = JSONNode::ParseString(str.c_str());
        ASSERT_TRUE(node);
        node.read("val", read_val);
    }
    ASSERT_EQ(write_val, read_val);
}

TEST_F(JSONNodeTests, CheckIfHasKey) {
    auto node = JSONNode::ParseString("{\"obj\": {}}");

    ASSERT_FALSE(node.hasKey("obj1"));

    ASSERT_TRUE(node.hasKey("obj"));
}

TEST_F(JSONNodeTests, CheckWriteArray) {
    JSONNode arrayNode;
    arrayNode.write(1);
    arrayNode.write(2);
    arrayNode.write(3);

    JSONNode node;
    node.write("array", arrayNode);

    auto buffer = node.flushToBuffer();
    ASSERT_TRUE(buffer);

    auto resNode = JSONNode::ParseBuffer(buffer);
    ASSERT_TRUE(resNode);

    auto resArrayNode = resNode.object("array");
    ASSERT_TRUE(resArrayNode);
    ASSERT_EQ(resArrayNode.size(), 3u);

    int i = 1;
    for(auto& elemNode : resArrayNode) {
        int val;
        elemNode.read(val);
        ASSERT_EQ(i, val);
        ++i;
    }
}