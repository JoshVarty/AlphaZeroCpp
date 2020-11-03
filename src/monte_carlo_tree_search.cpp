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
    vector<int> actions;
    vector<int> visitCounts;

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