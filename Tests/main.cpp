#include "gtest/gtest.h"

#include <Environment.h>

int main(int argc, char *argv[])
{
    ::testing::AddGlobalTestEnvironment(new TestsEnvironment());
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
