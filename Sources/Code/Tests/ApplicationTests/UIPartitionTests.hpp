#ifndef __UI_PARTITION_TESTS_HPP__
#define __UI_PARTITION_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIPartition;

class UIPartitionTests : public ConsoleAppTests {
public:

    UIPartition* createUIPartition();

protected:

    std::vector<std::unique_ptr<GameObject>> tempObject;
};

#endif /* __UI_PARTITION_TESTS_HPP__ */