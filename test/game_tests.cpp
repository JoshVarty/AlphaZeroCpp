#include <gtest/gtest.h>
#include <game.h>


TEST(Connect2Tests, EnsureInitialBoardIsEmpty) { 

    Connect2Game game;

    std::vector<int> result = game.GetInitBoard();
    auto x = 5;
}
 
 