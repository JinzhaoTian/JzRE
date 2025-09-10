#include <gtest/gtest.h>

/**
 * @brief main test function for running all unit tests.
 */
int main(int argc, char **argv)
{
    // 初始化Google Test框架
    ::testing::InitGoogleTest(&argc, argv);

    // 运行所有测试
    return RUN_ALL_TESTS();
}
