#include <vector>

using namespace std;


struct State {
    vector<int> board;
    int player;
};

class Connect2Game {

public:
    Connect2Game();
    vector<int> GetInitBoard();
    int GetBoardSize() { return columns; }
    int GetActionSize() { return columns; }
    State GetNextState(vector<int> board, int player, int action);
    vector<int> GetValidMoves(vector<int> board);
    bool HasLegalMoves(vector<int> board);
    bool IsWin(vector<int> board, int player);
    int GetRewardForPlayer(vector<int> board, int player);

private:
    int columns = 4;        // The number of columns in our game
    int num_to_win = 2;     // The required number of adjacent tokens to win
};