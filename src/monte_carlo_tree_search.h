#include <torch/torch.h>
#include <game.h>
#include <model.h>
#include <random>

class Node {
public:
    Node(float prior, int toPlay, int action);
    bool IsExpanded();
    float GetValue();
    int SelectAction(float temperature);
    Node SelectChild();
    void Expand(std::vector<int> state, int toPlay, std::vector<float> actionProbs);
private:
    int _visitCount = 0;
    int _toPlay = 0;
    int _action = -1;
    float _prior = 0;
    float _valueSum = 0;
    std::vector<Node> _children;
    std::vector<int> _state;
    std::default_random_engine _generator;
    float _UcbScore(Node parent, Node child);
};

class MCTS {
public:
    MCTS(Connect2Game game, Connect2Model model, int numSimluations);
    Node Run(Connect2Model model, std::vector<int> state, int toPlay);
private:
    void BackPropagate(std::vector<Node> searchPath, float value, int toPlay);
    Connect2Game _game;
    Connect2Model _model;
    int _numSimulations;
};