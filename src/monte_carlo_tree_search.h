#ifndef MCTS_H
#define MCTS_H

#include <game.h>
#include <model.h>
#include <torch/torch.h>

#include <random>

class Node {
 public:
  Node(float prior, int toPlay, int action);

  int GetVisitCount() const { return visit_count_; };
  int GetPlayerId() const { return to_play_; };
  int GetAction() const { return action_; };
  std::vector<int> GetState() const { return state_; };
  void AccumulateValue(float val) { value_sum_ += val; };
  void IncrementVisitCount() { ++visit_count_; };

  void Expand(const std::vector<int>& state, int to_play,
              const std::vector<float>& action_probs);
  bool IsExpanded();
  float GetValue();
  int SelectAction(float temperature);
  Node* SelectChild();
  Node* GetChild(int index) {
    CHECK_LT(index, children_.size());
    return children_[index].get();
  }

 private:
  int visit_count_ = 0;
  int to_play_ = 0;
  int action_ = -1;
  float prior_ = 0;
  float value_sum_ = 0;
  std::vector<std::unique_ptr<Node>> children_;
  std::vector<int> state_;
  std::default_random_engine generator_;
  float UcbScore_(Node* parent, Node* child);
};

class MCTS {
 public:
  MCTS(ConnectXGame& game, Model& model);

  static std::vector<float> MaskInvalidMovesAndNormalize(
      std::vector<float>& action_probs, const std::vector<int>& valid_moves);
  static void Backup(const std::vector<Node*>& search_path, float value, int to_play);

  Node* Run(std::vector<int>& state, int to_play,
            int num_simulations);

 private:
  ConnectXGame& game_;
  Model& model_;
};

#endif /* MCTS_H */