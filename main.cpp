#include <torch/torch.h>

#include <iostream>

#include "game.h"
#include "model.h"
#include "trainer.h"

int main() {
  auto game = Connect2Game();
  auto board_size = game.GetBoardSize();
  auto action_size = game.GetActionSize();

  torch::Device device(torch::kCPU);
  if(torch::cuda::is_available()) {
    std::cout << "Setting device: torch::kCUDA" << std::endl;
    device = torch::Device(torch::kCUDA);
  } else {
    std::cout << "Setting device: torch::kCPU" << std::endl;
  }

  auto model = Connect2Model(board_size, action_size, device);
  auto options = TrainerOptions();
  options.batch_size = 64;
  options.num_episodes = 100;
  options.num_epochs = 1;
  options.num_simulations = 100;
  options.training_iterations = 500;
  auto trainer = Trainer(game, model, options);

  trainer.Learn();
}
