/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <gtest/gtest.h>

/**
 * @brief main test function for running all unit tests.
 */
int main(int argc, char **argv)
{
    // init Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // run all unit test
    return RUN_ALL_TESTS();
}
