#ifndef GAME_H
#define GAME_H

#include <optional>
#include <vector>

struct StateAndPlayer {
  std::vector<int> board;
  int player;
};

class ConnectXGame {
 public:
  virtual std::vector<int> GetInitBoard() const = 0;
  virtual StateAndPlayer GetNextState(const std::vector<int>& board, int player,
                                      int action) const = 0;
  virtual std::vector<int> GetValidMoves(
      const std::vector<int>& board) const = 0;
  virtual bool HasLegalMoves(const std::vector<int>& board) const = 0;
  virtual bool IsWin(const std::vector<int>& board, int player) const = 0;
  virtual std::optional<int> GetRewardForPlayer(const std::vector<int>& board,
                                                int player) const = 0;
  virtual std::vector<int> GetCanonicalBoard(const std::vector<int>& board,
                                             int player) const = 0;
};

class Connect2Game : public ConnectXGame {
 public:
  Connect2Game() : rows(1), columns(4), num_to_win(2) {}

  int GetBoardSize() { return rows * columns; }
  int GetActionSize() { return rows * columns; }

  std::vector<int> GetInitBoard() const override;
  StateAndPlayer GetNextState(const std::vector<int>& board, int player,
                              int action) const override;
  std::vector<int> GetValidMoves(const std::vector<int>& board) const override;
  bool HasLegalMoves(const std::vector<int>& board) const override;
  bool IsWin(const std::vector<int>& board, int player) const override;
  std::optional<int> GetRewardForPlayer(const std::vector<int>& board,
                                        int player) const override;
  std::vector<int> GetCanonicalBoard(const std::vector<int>& board,
                                     int player) const override;

 private:
  int rows = 0;        // The number of rows on our board
  int columns = 0;     // The number of columns on our board
  int num_to_win = 0;  // The required number of adjacent tokens to win
};

#endif /* GAME_H */