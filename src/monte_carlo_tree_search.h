#include <torch/torch.h>
#include <game.h>
#include <model.h>

class Node {
public:
    Node(float prior, int toPlay);
    bool IsExpanded();
    float GetValue();
    int SelectAction(float temperature);
    void SelectChild();
    void Expand(std::vector<int> state, int toPlay, std::vector<float> actionProbs);
private:
};

class MCTS {
public:
    MCTS(Connect2Game game, Connect2Model model, int numSimluations);
    Node Run(Connect2Model model, std::vector<int> state, int toPlay);
private:
    void BackPropagate(std::vector<Node> searchPath, float value, int toPlay);
};