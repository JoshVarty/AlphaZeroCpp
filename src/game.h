#ifndef GAME_H
#define GAME_H

#include <vector>

struct StateAndPlayer {
    std::vector<int> board;
    int player;
};

class Connect2Game {

public:
    Connect2Game() { }
    std::vector<int> GetInitBoard();
    int GetBoardSize() { return columns; }
    int GetActionSize() { return columns; }
    StateAndPlayer GetNextState(std::vector<int> board, int player, int action);
    std::vector<int> GetValidMoves(std::vector<int> board);
    bool HasLegalMoves(std::vector<int> board);
    bool IsWin(std::vector<int> board, int player);
    int GetRewardForPlayer(std::vector<int> board, int player);
    std::vector<int> GetCanonicalBoard(std::vector<int> board, int player);

private:
    int columns = 4;        // The number of columns in our game
    int num_to_win = 2;     // The required number of adjacent tokens to win
};

#endif /* GAME_H */