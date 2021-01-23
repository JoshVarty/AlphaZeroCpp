#include <monte_carlo_tree_search.h>

#include <random>

Node::Node(float prior, int to_play, int action)
    : prior_(prior), to_play_(to_play), action_(action) {}

bool Node::IsExpanded() { return children_.size() > 0; }

float Node::GetValue() {
  return visit_count_ == 0 ? 0 : value_sum_ / visit_count_;
}

int Node::SelectAction(float temperature) {
  std::vector<int> actions, visit_counts;

  int max_action = -1, max_visit_count = -1;

  for (size_t i = 0; i < children_.size(); ++i) {
    Node* child = children_[i].get();
    actions.push_back(child->action_);
    visit_counts.push_back(child->visit_count_);

    if (child->visit_count_ > max_visit_count) {
      max_action = i;
      max_visit_count = child->visit_count_;
    }
  }

  if (temperature == 0) {
    // For zero temperature, we select the action with the highest visitCount
    return max_action;
  } else {
    // otherwise we select randomly from the visitCount distribution
    std::discrete_distribution<int> distr(visit_counts.begin(),
                                          visit_counts.end());
    int random_index = distr(generator_);
    return actions[random_index];
  }
}

float Node::UcbScore_(Node* parent, Node* child) {
  float prior_score =
      child->prior_ * sqrt(parent->visit_count_) / (child->visit_count_ + 1);

  float value_score = 0;
  if (child->visit_count_ > 0) {
    // Recall that the child's value is from the perspective of the opposing
    // player so we must negate it Eg. A win (+1) for the opponent is worth a
    // loss (-1) for us.
    value_score = -child->GetValue();
  }

  return value_score + prior_score;
}

Node* Node::SelectChild() {
  float best_score = std::numeric_limits<float>::min();
  Node* best_child = children_.front().get();

  for (auto&& child_ptr : children_) {
    auto child = child_ptr.get();
    auto score = this->UcbScore_(this, child);
    if (score > best_score) {
      best_score = score;
      best_child = child;
    }
  }

  return best_child;
}

void Node::Expand(const std::vector<int>& state, int to_play,
                  const std::vector<float>& action_probs) {
  this->to_play_ = to_play;
  this->state_ = state;

  for (size_t action = 0; action < action_probs.size(); ++action) {
    auto prior_prob = action_probs[action];
    if (prior_prob != 0.0f) {
      auto new_child = std::make_unique<Node>(prior_prob, -to_play, action);
      this->children_.push_back(std::move(new_child));
    }
  }
}

MCTS::MCTS(ConnectXGame& game, Model& model) : game_(game), model_(model) {}

std::vector<float> MCTS::MaskInvalidMovesAndNormalize(
    std::vector<float>& action_probs, const std::vector<int>& valid_moves) {
  // Mask out invalid moves
  std::transform(action_probs.begin(), action_probs.end(), valid_moves.begin(),
                 action_probs.begin(), std::multiplies<float>());

  // Normalize remaining probabilities
  float kEps = 1e-9;
  float sum_of_valid_probs =
      std::accumulate(action_probs.begin(), action_probs.end(), 0.0) + kEps;

  std::transform(action_probs.begin(), action_probs.end(), action_probs.begin(),
                 [&sum_of_valid_probs](float prob) -> float {
                   return prob / sum_of_valid_probs;
                 });

  return action_probs;
}

Node* MCTS::Run(std::vector<int>& state, int to_play,
                int num_simulations) {
  Node* root = new Node(0, to_play, -1);

  // Expand root
  auto result = model_.predict(state);
  auto action_probs = result.action_probs;
  auto value = result.value;
  auto valid_moves = this->game_.GetValidMoves(state);
  action_probs = MaskInvalidMovesAndNormalize(action_probs, valid_moves);
  root->Expand(state, to_play, action_probs);

  for (int i = 0; i < num_simulations; ++i) {
    Node* node = root;
    std::vector<Node*> search_path({node});

    // SELECT
    while (node->IsExpanded()) {
      node = node->SelectChild();
      search_path.push_back(node);
    }

    Node* parent = search_path[search_path.size() - 2];
    state = parent->GetState();
    // Now we're at a leaf node and we would like to expand
    // Players always play from their own perspective
    auto next_state_and_player = this->game_.GetNextState(
        state, /*player=*/1, /*action=*/node->GetAction());
    auto next_state = next_state_and_player.board;
    // Get the board from the perspective of the other player
    next_state = this->game_.GetCanonicalBoard(next_state, /*player=*/-1);

    // The value of the new state from the perspective of the other player
    auto opt_value = this->game_.GetRewardForPlayer(next_state, /*player=*/1);

    if (!opt_value.has_value()) {
      // If the game has not ended:
      // EXPAND
      auto pred = model_.predict(next_state);
      action_probs = pred.action_probs;
      value = pred.value;
      auto valid_moves = this->game_.GetValidMoves(next_state);
      // Mask and normalize
      action_probs = MaskInvalidMovesAndNormalize(action_probs, valid_moves);
      node->Expand(next_state, -parent->GetPlayerId(), action_probs);
    } else {
      value = opt_value.value();
    }

    this->Backup(search_path, value, -parent->GetPlayerId());
  }

  return root;
}

void MCTS::Backup(const std::vector<Node*>& search_path, float value, int to_play) {
  for (auto& node : search_path) {
    if (node->GetPlayerId() == to_play) {
      node->AccumulateValue(value);
    } else {
      node->AccumulateValue(-value);
    }

    node->IncrementVisitCount();
  }
}
