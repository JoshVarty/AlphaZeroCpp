#include <gtest/gtest.h>
#include "game_tests.cpp"
#include "model_tests.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}