#include "JSONNodeTests.hpp"
#include "Core/JSONNode.hpp"
#include "Core/Buffer.hpp"

#include <map>
#include <vector>

TEST_F(JSONNodeTests, ParseEmptyBuffer) {
    Buffer buff;
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
    node.value("int", intVal);
    ASSERT_EQ(intVal, 1);

    float floatVal = 0;
    node.value("float", floatVal);
    ASSERT_EQ(floatVal, 1.f);

    std::string strVal;
    node.value("str", strVal);
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
    begIt->value(intVal);
    ASSERT_EQ(intVal, 1);
}

TEST_F(JSONNodeTests, CheckForLoop) {
    auto node = JSONNode::ParseString("{\"obj1\": 1, \"obj2\": 2}");

    std::map<std::string, int> keyVal;
    size_t totalIteration = 0;
    int intVal = 0;
    for(const auto& item : node) {
        item.value(intVal);
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
        elem.value(intVal);
        intVals.push_back(intVal);
    }

    ASSERT_EQ(intVals.size(), 10u);
    for(size_t i=0u,sz=intVals.size(); i<sz; ++i) {
        ASSERT_EQ(intVals[i], i+1);
    }
}

TEST_F(JSONNodeTests, CheckAccessToInvalidElements) {
    auto node = JSONNode::ParseString("{\"obj\": {}}");

    ASSERT_TRUE(node);
    ASSERT_EQ(node.size(), 1);

    auto objNode = node.object("invalid");
    ASSERT_FALSE(objNode);

    std::string str = "empty_val";
    objNode.value("invalid", str);
    ASSERT_STREQ(str.c_str(), "empty_val");
    objNode.value(str);
    ASSERT_STREQ(str.c_str(), "empty_val");

    float floatVal = -1.f;
    objNode.value("invalid", floatVal);
    ASSERT_EQ(floatVal, -1.f);
    objNode.value(floatVal);
    ASSERT_EQ(floatVal, -1.f);

    int intVal = -1;
    objNode.value("invalid", intVal);
    ASSERT_EQ(intVal, -1);
    objNode.value(intVal);
    ASSERT_EQ(intVal, -1);

    size_t iterCount = 0;
    for(const auto& elem : objNode) {
        ++iterCount;
    }

    ASSERT_EQ(iterCount, 0);
    ASSERT_EQ(objNode.begin(), objNode.end());
}