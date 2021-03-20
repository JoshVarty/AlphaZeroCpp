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
                         options_.num_simulations);
    
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

    // TODO (Are the canonical boards correct? They don't seem to be changing properly)
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


void Trainer::Train(const std::vector<Example>& examples) {
  torch::optim::AdamOptions opt;
  opt = opt.lr(5e-4);
  auto optimizer = torch::optim::Adam(this->model_.parameters(), opt);

  auto pi_losses = std::vector<float>();
  auto v_losses = std::vector<float>();

  this->model_.train();

  std::cout << "NUM EXAMPLES:\t" << examples.size() << std::endl;
  for (int i = 0; i < options_.num_epochs; ++i) {

    uint32_t batch_idx = 0;

    auto num_batches = static_cast<int>(examples.size() 
                                        / options_.batch_size);

    float boards[options_.batch_size][game_.GetBoardSize()];
    float target_pis[options_.batch_size][game_.GetBoardSize()];
    float target_vis[options_.batch_size];

    while (batch_idx < num_batches) {
      int start_idx = batch_idx * options_.batch_size;

      // Build an input tensor
      // TODO: (joshvarty) If anyone knows the proper way to do this,
      // please let me know
      for(int i = 0; i < options_.batch_size; ++i) {
        auto i_offset = i + start_idx;
        for(int j = 0; j < game_.GetActionSize(); ++j) {
          boards[i][j] = static_cast<float>(examples[i_offset].canonical_board[j]);
          target_pis[i][j] = examples[i_offset].action_probs[j];
        }
        target_vis[i] = examples[i].reward;
      }

      auto opt = torch::TensorOptions().device(torch::kCPU);
      torch::Tensor board_tensor = torch::from_blob(boards, {options_.batch_size, game_.GetActionSize()}, opt.dtype(torch::kFloat32));
      torch::Tensor pis_tensor = torch::from_blob(target_pis, {options_.batch_size, game_.GetActionSize()}, opt.dtype(torch::kFloat32));
      torch::Tensor vis_tensor = torch::from_blob(target_vis, {options_.batch_size}, opt.dtype(torch::kFloat32));
      // TODO: (#13) Create Tensor on the device instead of moving it there
      board_tensor = board_tensor.to(this->model_.device);
      pis_tensor = pis_tensor.to(this->model_.device);
      vis_tensor = vis_tensor.to(this->model_.device);

      // Get output and loss from model
      auto probs_and_value = this->model_.forward(board_tensor);
      auto out_pis = probs_and_value.action_probs;
      auto out_v = probs_and_value.value;
      auto l_pi = this->GetProbabilityLoss(pis_tensor, out_pis);
      auto l_vi = this->GetValueLoss(vis_tensor, out_v);
      auto total_loss = l_pi + l_vi;

      // Backprop
      optimizer.zero_grad();
      total_loss.backward();
      optimizer.step();

      // Keep track of losses
      auto avg_l_vi = l_vi.cpu().item<float>();

      pi_losses.push_back(l_pi.cpu().item<float>());
      v_losses.push_back(l_vi.cpu().item<float>());

      ++batch_idx;
    }

    auto avg_p_loss = std::accumulate(pi_losses.begin(), pi_losses.end(), 0.0) / pi_losses.size();
    auto avg_v_loss = std::accumulate(v_losses.begin(), v_losses.end(), 0.0) / v_losses.size();
    std::cout << "Avg p_loss:\t" << avg_p_loss << std::endl;
    std::cout << "Avg v_loss:\t" << avg_v_loss << std::endl;
  }
}


void Trainer::Learn() {
  for(uint32_t i = 0; i < options_.training_iterations; ++i) {
    std::cout << i << "/" << options_.training_iterations << std::endl;

    std::vector<Example> training_examples;

    for (uint32_t episode = 0; episode < options_.num_episodes; ++episode) {
      auto iter_training_examples = this->ExecuteEpisode();
      training_examples.insert(training_examples.end(), 
                               iter_training_examples.begin(),
                               iter_training_examples.end());
    }

    std::random_shuffle(training_examples.begin(), training_examples.end());
    this->Train(training_examples);
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
