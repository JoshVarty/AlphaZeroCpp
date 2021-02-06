#include "trainer.h"


std::vector<Example> Trainer::ExecuteEpisode() {
  std::vector<Example> train_examples;
  int current_player = 1;
  auto state = this->game_.GetInitBoard();

  while (true) {
    auto canonical_board = this->game_.GetCanonicalBoard(state, 
                                                         current_player);
    auto mcts = MCTS(this->game_, this->model_);
    auto root = mcts.Run(canonical_board, current_player, 
                         this->num_simulations_);
    
    auto action_probs = std::vector<float>(this->game_.GetActionSize(), 0);
    for(auto&& child : root->Children) {
      action_probs[child->GetAction()] = child->GetVisitCount();
    }

    // Normalize visit counts into probability distribution
    const float kEps = 1e-9;
    float sum_of_valid_probs =
        std::accumulate(action_probs.begin(), action_probs.end(), 0.0) + kEps;
    std::transform(action_probs.begin(), action_probs.end(), action_probs.begin(),
                   [&sum_of_valid_probs](float prob) -> float {
                     return prob / sum_of_valid_probs;
                   });

    // Adding default reward of zero for now. Once the game completes, 
    // we will correct the example with the correct reward
    train_examples.push_back({canonical_board, current_player, action_probs, 0});

    auto action = root->SelectAction(/*temperature=*/0);
    auto state_and_player = game_.GetNextState(state, current_player, action);
    state = state_and_player.board;
    current_player = state_and_player.player;
    
    //reward = self.game.get_reward_for_player(state, current_player)
    auto reward = this->game_.GetRewardForPlayer(state, current_player);

    if (reward.has_value()) {
      for(auto& example : train_examples) {
        //TODO: Check if this works
        example.reward = reward.value() * ((example.current_player == current_player) * 1 +
                                           (example.current_player != current_player) * -1);
      }

      return train_examples;
    }
  }
}