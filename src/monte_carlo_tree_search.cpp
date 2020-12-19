#include <monte_carlo_tree_search.h>
#include <random>

Node::Node(float prior, int toPlay, int action) :
_prior(prior),
_toPlay(toPlay),
_action(action)
{
}

bool Node::IsExpanded() {
    return _children.size() > 0;
}

float Node::GetValue() {
    if (_visitCount == 0) {
        return 0;
    }

    return _valueSum / _visitCount;
}

int Node::SelectAction(float temperature) {
    std::vector<int> actions;
    std::vector<int> visitCounts;

    int maxAction = -1;
    int maxVisitCount = -1;

    for (int i = 0; i < _children.size(); i++) {
        auto child =  _children[i];
        actions.push_back(child._action);
        visitCounts.push_back(child._visitCount);

        if (child._visitCount > maxVisitCount) {
            maxAction = i;
            maxVisitCount = child._visitCount;
        }
    }

    if (temperature == 0) {
        // For zero temperature, we select the action with the highest visitCount
        return maxAction;
    }
    else {
        // otherwise we select randomly from the visitCount distribution
        std::discrete_distribution<int> distr(visitCounts.begin(), visitCounts.end());
        int random_index = distr(_generator);
        return actions[random_index];
    }
}


float Node::_UcbScore(Node parent, Node child) {

    float prior_score = child._prior * sqrt(parent._visitCount) / (child._visitCount + 1);
    float value_score = 0;
    if (child._visitCount > 0) {
        // Recall that the child's value is from the perspective of the opposing player so we must negate it
        // Eg. A win (+1) for the opponent is worth a loss (-1) for us.
        value_score = -child.GetValue();
    }

    return value_score + prior_score;
}

Node Node::SelectChild() {
    float bestScore = std::numeric_limits<float>::min();
    Node bestChild = _children.front();

    for (auto child : _children) {
        auto score = this->_UcbScore(*this, child);
        if (score > bestScore) {
            bestScore = bestScore;
            bestChild = child;
        }
    }

    return bestChild;
}

void Node::Expand(std::vector<int> state, int toPlay, std::vector<float> actionProbs) {

    this->_toPlay = toPlay;
    this->_state = state;

    for(int action = 0; action < actionProbs.size(); action++) {
        auto priorProb = actionProbs[action];

        if (priorProb != 0.0f) {
            Node newChild(priorProb, toPlay, action);
            this->_children.push_back(newChild);
        }
    }
}

MCTS::MCTS(Connect2Game game, Connect2Model model, int numSimluations) :
 _game(game),
 _model(model),
 _numSimulations(numSimluations) { 
}

std::vector<float> MCTS::MaskInvalidMovesAndNormalize(std::vector<float> actionProbs, std::vector<int> validMoves) {
    // Mask out invalid moves
    std::transform(actionProbs.begin(), actionProbs.end(),
                    validMoves.begin(), 
                    actionProbs.begin(),
                    std::multiplies<float>());

    // // Normalize remaining probabilities
    float kEps = 1e-9;
    float sumOfValidProbs = std::accumulate(actionProbs.begin(), actionProbs.end(), 0.0) + kEps;
    
    std::transform(actionProbs.begin(), actionProbs.end(),
                    actionProbs.begin(),
                    [&sumOfValidProbs] (float prob) -> float { return prob / sumOfValidProbs; });

    return actionProbs;
}



Node MCTS::Run(Connect2Model model, std::vector<int> state, int toPlay, int numSimulations) {

    auto root = Node(0, toPlay, -1);

    // Expand root
    auto result = model.predict(state);
    auto actionProbs = result.actionProbs;
    auto value = result.value;
    auto validMoves = this->_game.GetValidMoves(state);
    actionProbs = MaskInvalidMovesAndNormalize(actionProbs, validMoves);
    root.Expand(state, toPlay, actionProbs);

    for (int i = 0; i < numSimulations; i++) {            
        Node node = root;
        std::vector<Node*> searchPath = { &node };

        // SELECT
        while (node.IsExpanded()) {
            node = node.SelectChild();
            searchPath.push_back(&node);
        }

        Node parent = *(searchPath[searchPath.size() - 2]);
        state = parent.GetState();
        // Now we're at a leaf node and we would like to expand
        // Players always play from their own perspective
        auto nextStateAndPlayer = this->_game.GetNextState(state, /*player=*/1, /*action=*/node.GetAction());
        auto nextState = nextStateAndPlayer.board;
        // Get the board from the perspective of the other player
        nextState = this->_game.GetCanonicalBoard(nextState, /*player=*/-1);

        // The value of the new state from the perspective of the other player
        auto optValue = this->_game.GetRewardForPlayer(nextState, /*player=*/1);

        if (optValue.has_value()) {
            value = optValue.value();
            // If the game has not ended:
            // EXPAND
            auto pred = model.predict(nextState);
            actionProbs = pred.actionProbs;
            value = pred.value;
            auto valid_moves = this->_game.GetValidMoves(nextState);
            // Mask and normalize
            actionProbs = MaskInvalidMovesAndNormalize(actionProbs, validMoves);
            node.Expand(nextState, value, actionProbs);
        }

        this->Backup(searchPath, value, parent.GetPlayerId() * -1);
    }

    return root;
}

void MCTS::Backup(std::vector<Node*> searchPath, float value, int toPlay) {
    for (auto& node : searchPath) {
        if (node->GetPlayerId() == toPlay) {
            node->AccumulateValue(value);
        } else {
            node->AccumulateValue(-value);
        }

        node->IncrementVisitCount();
    }
}

