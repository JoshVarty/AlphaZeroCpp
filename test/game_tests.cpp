#include <game.h>
#include <gtest/gtest.h>

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
  std::vector<int> original_board = {0, 0, -1, 0};
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

TEST(Connect2Tests, GetValidMovesWorks) {
  Connect2Game game;
  std::vector<int> original_board = {0, 0, -1, 0};

  auto valid_moves = game.GetValidMoves(original_board);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 0);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], -1);
  ASSERT_EQ(original_board[3], 0);

  // Ensure valid moves are correct
  ASSERT_EQ(valid_moves[0], 1);
  ASSERT_EQ(valid_moves[1], 1);
  ASSERT_EQ(valid_moves[2], 0);
  ASSERT_EQ(valid_moves[3], 1);
}

TEST(Connect2Tests, GetValidMovesWorks_NoMovesAvailable) {
  Connect2Game game;
  std::vector<int> original_board = {1, -1, 1, -1};

  auto valid_moves = game.GetValidMoves(original_board);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], -1);
  ASSERT_EQ(original_board[2], 1);
  ASSERT_EQ(original_board[3], -1);

  // Ensure valid moves are correct
  ASSERT_EQ(valid_moves[0], 0);
  ASSERT_EQ(valid_moves[1], 0);
  ASSERT_EQ(valid_moves[2], 0);
  ASSERT_EQ(valid_moves[3], 0);
}

TEST(Connect2Tests, HasLegalMoves) {
  Connect2Game game;
  std::vector<int> original_board = {0, -1, 0, 0};

  auto hasLegalMoves = game.HasLegalMoves(original_board);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 0);
  ASSERT_EQ(original_board[1], -1);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);

  ASSERT_TRUE(hasLegalMoves);
}

TEST(Connect2Tests, HasLegalMoves_NoMoves) {
  Connect2Game game;
  std::vector<int> original_board = {1, -1, 1, -1};

  auto hasLegalMoves = game.HasLegalMoves(original_board);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], -1);
  ASSERT_EQ(original_board[2], 1);
  ASSERT_EQ(original_board[3], -1);

  ASSERT_FALSE(hasLegalMoves);
}

TEST(Connect2Tests, IsWin_ValidMovesRemain) {
  Connect2Game game;
  std::vector<int> original_board = {0, 0, 0, 0};
  int player = 1;

  auto isWin = game.IsWin(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 0);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);

  ASSERT_FALSE(isWin);
}

TEST(Connect2Tests, IsWin_Draw) {
  Connect2Game game;
  std::vector<int> original_board = {1, -1, 1, -1};
  int player = 1;

  auto isWin = game.IsWin(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], -1);
  ASSERT_EQ(original_board[2], 1);
  ASSERT_EQ(original_board[3], -1);

  ASSERT_FALSE(isWin);
}

TEST(Connect2Tests, IsWin_Loss) {
  Connect2Game game;
  std::vector<int> original_board = {1, 1, -1, 1};
  int player = -1;

  auto isWin = game.IsWin(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], 1);
  ASSERT_EQ(original_board[2], -1);
  ASSERT_EQ(original_board[3], 1);

  ASSERT_FALSE(isWin);
}

TEST(Connect2Tests, IsWin_Win) {
  Connect2Game game;
  std::vector<int> original_board = {1, 1, -1, 1};
  int player = 1;

  auto isWin = game.IsWin(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], 1);
  ASSERT_EQ(original_board[2], -1);
  ASSERT_EQ(original_board[3], 1);

  ASSERT_TRUE(isWin);
}

TEST(Connect2Tests, GetRewardForPlayer_Win) {
  Connect2Game game;
  std::vector<int> original_board = {1, 1, -1, 0};
  int player = 1;

  auto reward = game.GetRewardForPlayer(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], 1);
  ASSERT_EQ(original_board[2], -1);
  ASSERT_EQ(original_board[3], 0);

  ASSERT_EQ(reward, 1);
}

TEST(Connect2Tests, GetRewardForPlayer_Loss) {
  Connect2Game game;
  std::vector<int> original_board = {-1, -1, 1, 0};
  int player = 1;

  auto reward = game.GetRewardForPlayer(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], -1);
  ASSERT_EQ(original_board[1], -1);
  ASSERT_EQ(original_board[2], 1);
  ASSERT_EQ(original_board[3], 0);

  ASSERT_EQ(reward, -1);
}

TEST(Connect2Tests, GetRewardForPlayer_NoWin) {
  Connect2Game game;
  std::vector<int> original_board = {0, 0, 1, 0};
  int player = 1;

  auto reward = game.GetRewardForPlayer(original_board, player);

  // No reward should be returned because the game isn't finised
  ASSERT_EQ(reward, std::nullopt);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 0);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 1);
  ASSERT_EQ(original_board[3], 0);
}

TEST(Connect2Tests, GetCanonicalBoard_Empty) {
  Connect2Game game;
  std::vector<int> original_board = {0, 0, 0, 0};
  int player = 1;

  auto next_board = game.GetCanonicalBoard(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 0);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);

  // Ensure next board is correct
  ASSERT_EQ(original_board[0], 0);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);
}

TEST(Connect2Tests, GetCanonicalBoard_NoChange) {
  Connect2Game game;
  std::vector<int> original_board = {1, 0, 0, 0};
  int player = 1;

  auto next_board = game.GetCanonicalBoard(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);

  // Ensure next board is correct
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);
}

TEST(Connect2Tests, GetCanonicalBoard_TogglePointOfView) {
  Connect2Game game;
  std::vector<int> original_board = {1, 0, 0, 0};
  int player = -1;

  auto next_board = game.GetCanonicalBoard(original_board, player);

  // Ensure original board is unchanged
  ASSERT_EQ(original_board[0], 1);
  ASSERT_EQ(original_board[1], 0);
  ASSERT_EQ(original_board[2], 0);
  ASSERT_EQ(original_board[3], 0);

  // Ensure next board is correct
  ASSERT_EQ(next_board[0], -1);
  ASSERT_EQ(next_board[1], 0);
  ASSERT_EQ(next_board[2], 0);
  ASSERT_EQ(next_board[3], 0);
}