#include <gtest/gtest.h>
#include <game.h>


TEST(Connect2Tests, EnsureInitialBoardIsEmpty) { 
    Connect2Game game;
    auto empty_board = game.GetInitBoard();

    ASSERT_EQ(empty_board.size(), 4);
    ASSERT_EQ(empty_board[0], 0);
    ASSERT_EQ(empty_board[1], 0);
    ASSERT_EQ(empty_board[2], 0);
    ASSERT_EQ(empty_board[3], 0);
}

TEST(Connect2Tests, GetNextStateWorks) {

    Connect2Game game;
    vector<int> original_board = {0, 0, -1, 0};
    int player = 1;
    int action = 0;
    auto nextState = game.GetNextState(original_board, player, action);

    // Ensure original board is unchanged
    ASSERT_EQ(original_board[0], 0);
    ASSERT_EQ(original_board[1], 0);
    ASSERT_EQ(original_board[2], -1);
    ASSERT_EQ(original_board[3], 0);

    // Ensure new board is correct
    ASSERT_EQ(nextState.board[0], 1);
    ASSERT_EQ(nextState.board[1], 0);
    ASSERT_EQ(nextState.board[2], -1);
    ASSERT_EQ(nextState.board[3], 0);

    // Ensure other player is playing
    ASSERT_EQ(nextState.player, -1);








}
 