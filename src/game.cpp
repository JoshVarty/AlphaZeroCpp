#include "game.h"
#include <algorithm>
#include <functional>


std::vector<int> Connect2Game::GetInitBoard() const {
    return {0, 0, 0, 0};
}

StateAndPlayer Connect2Game::GetNextState(std::vector<int> board, int player, int action) const {
    std::vector<int> new_board(board);

    new_board[action] = player;

    StateAndPlayer nextStateAndPlayer = {new_board, -player };
    return nextStateAndPlayer;
}

std::vector<int> Connect2Game::GetValidMoves(std::vector<int> board) const {
    std::vector<int> valid_moves = {0, 0, 0, 0};

    for(int i = 0; i < columns; i++) {
        if (board[i] == 0) {
            valid_moves[i] = 1;
        }
    }

    return valid_moves;
}

bool Connect2Game::HasLegalMoves(std::vector<int> board) const {

    for (int i = 0; i < columns; i++) {
        if (board[i] == 0)
            return true;
    }

    return false;
}

bool Connect2Game::IsWin(std::vector<int> board, int player) const {
    int count = 0;

    for(int i = 0; i < columns; i++) {
        if(board[i] == player) {
            count += 1;
        }
        else {
            count = 0;
        }

        if (count == num_to_win) {
            return true;
        }
    }

    return false;
}

std::optional<int> Connect2Game::GetRewardForPlayer(std::vector<int> board, int player) const {

    if (IsWin(board, player)) {
        return 1;
    }
    if (IsWin(board, -player)) {
        return -1;
    }
    if (!HasLegalMoves(board)) {
        return 0;
    }

    return std::nullopt;
}

std::vector<int> Connect2Game::GetCanonicalBoard(std::vector<int> old_board, int player) const {
    std::vector<int> board(old_board);
    std::transform(board.begin(), board.end(), board.begin(),
               std::bind(std::multiplies<int>(), std::placeholders::_1, -1));

    return board;
}
