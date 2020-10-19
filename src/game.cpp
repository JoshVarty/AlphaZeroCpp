#include "game.h"
#include <algorithm>
#include <functional>


vector<int> Connect2Game::GetInitBoard() {
    return {0, 0, 0, 0};
}

State Connect2Game::GetNextState(vector<int> board, int player, int action) {
    vector<int> new_board(board);

    new_board[action] = player;

    auto nextState = State();
    nextState.board = new_board;
    nextState.player = -player;
    return nextState;
}

vector<int> Connect2Game::GetValidMoves(vector<int> board) {
    vector<int> valid_moves = {0, 0, 0, 0};

    for(int i = 0; i < columns; i++) {
        if (board[i] == 0) {
            valid_moves[i] = 1;
        }
    }

    return valid_moves;
}

bool Connect2Game::HasLegalMoves(vector<int> board) {

    for (int i = 0; i < columns; i++) {
        if (board[i] == 0)
            return true;
    }

    return false;
}

bool Connect2Game::IsWin(vector<int> board, int player) {
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

int Connect2Game::GetRewardForPlayer(vector<int> board, int player) {

    if (IsWin(board, player)) {
        return 1;
    }
    if (IsWin(board, -player)) {
        return -1;
    }
    if (HasLegalMoves(board)) {
        //TODO: We need a better sentinel value. Optional types?
        return 123;
    }

    // Draw
    return 0;
}

vector<int> Connect2Game::GetCanonicalBoard(vector<int> board, int player) {
    std::transform(board.begin(), board.end(), board.begin(),
               std::bind(std::multiplies<int>(), std::placeholders::_1, -1));

    return board;
}
