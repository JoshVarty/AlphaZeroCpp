#include <torch/torch.h>

#include <iostream>

#include "game.h"
#include "model.h"

int main() {
  auto game = Connect2Game();
  auto board_size = game.GetBoardSize();
  auto action_size = game.GetActionSize();

  torch::Device device(torch::kCPU);
  if(torch::cuda::is_available()) {
    std::cout << "Setting device: torch::kCUDA";
    device = torch::Device(torch::kCUDA);
  } else {
    std::cout << "Setting device: torch::kCPU";
  }

  auto model = Connect2Model(board_size, action_size, device);
  // TODO (joshvarty): Create trainer and learn!
}
