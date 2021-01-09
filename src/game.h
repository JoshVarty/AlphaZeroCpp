#ifndef GAME_H
#define GAME_H

#include <vector>
#include <optional>

struct StateAndPlayer {
    std::vector<int> board;
    int player;
};


class ConnectXGame {

public:
    virtual std::vector<int> GetInitBoard() const = 0;
    virtual StateAndPlayer GetNextState(std::vector<int> board, int player, int action) const = 0;
    virtual std::vector<int> GetValidMoves(std::vector<int> board) const = 0;
    virtual bool HasLegalMoves(std::vector<int> board) const = 0;
    virtual bool IsWin(std::vector<int> board, int player) const = 0;
    virtual std::optional<int> GetRewardForPlayer(std::vector<int> board, int player) const = 0;
    virtual std::vector<int> GetCanonicalBoard(std::vector<int> board, int player) const = 0;

protected:
    int rows = 0;           // The number of rows on our board
    int columns = 0;        // The number of columns on our board
    int num_to_win = 0;     // The required number of adjacent tokens to win
};

class Connect2Game : public ConnectXGame {
public:

    int GetBoardSize() { return rows * columns; }
    int GetActionSize() { return rows * columns; }

    Connect2Game() {
        // TODO: Use that constructor initializer thing
        this->rows = 1;
        this->columns = 4;
        this->num_to_win = 2;
    }
    
    std::vector<int> GetInitBoard() const override;
    StateAndPlayer GetNextState(std::vector<int> board, int player, int action) const override;
    std::vector<int> GetValidMoves(std::vector<int> board) const override;
    bool HasLegalMoves(std::vector<int> board) const override;
    bool IsWin(std::vector<int> board, int player) const override;
    std::optional<int> GetRewardForPlayer(std::vector<int> board, int player) const override;
    std::vector<int> GetCanonicalBoard(std::vector<int> board, int player) const override;
};

#endif /* GAME_H */