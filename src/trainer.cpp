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

void Trainer::Learn() {
  for(uint32_t i = 0; i < training_iterations_; ++i) {
    std::cout << i << "/" << training_iterations_ << std::endl;

    std::vector<Example> training_examples;

    for (uint32_t episode = 0; episode < number_of_episodes_; ++episode) {
      auto iter_training_examples = this->ExecuteEpisode();
      training_examples.insert(training_examples.end(), 
                               iter_training_examples.begin(),
                               iter_training_examples.end());
    }

    std::random_shuffle(training_examples.begin(), training_examples.end());
    //this->Train(training_examples);
    // TODO (joshvarty): Probably want to let people change this?
    std::string kFileName = "checkpoint";
    //this->SaveCheckpoint("checkpoints", kFileName);
  }
}

torch::Tensor Trainer::GetProbabilityLoss(torch::Tensor targets,
                                          torch::Tensor outputs) {
  auto loss = -(targets * torch::log(outputs)).sum(1);
  return loss.mean();
}

torch::Tensor Trainer::GetValueLoss(torch::Tensor targets,
                                    torch::Tensor outputs) {
  // loss = torch.sum((targets-outputs.view(-1))**2)/targets.size()[0]
  auto loss = torch::sum((targets - outputs).view(-1).pow(2)) / targets.size(0);
  return loss;
}

void Trainer::SaveCheckpoint(std::string folder, std::string filename) {
  // if (!std::experimental::filesystem::exists(folder)) {
  //   std::experimental::filesystem::create_directory(folder);
  // }

  // auto path = std::experimental::filesystem::path(folder);
  // path.append(filename);

  // torch::save(this->model_, path.string());
}
