#ifndef TRAINER_H
#define TRAINER_H

#include "game.h"
#include "model.h"
#include "monte_carlo_tree_search.h"

#include <torch/torch.h>

struct Example {
  std::vector<int> canonical_board;
  int current_player;
  std::vector<float> action_probs;
  int reward;
};

class Trainer {
  public:
    Trainer(Connect2Game game, Connect2Model model, uint32_t num_simulations) : 
      game_(game), model_(model), num_simulations_(num_simulations) {}

    std::vector<Example> ExecuteEpisode();
    void Learn();
    void Train(std::vector<Example> examples);
    torch::Tensor GetProbabilityLoss();
    torch::Tensor GetValueLoss();
    void SaveCheckpoint(std::string folder, std::string filename);

  private:
    Connect2Game game_;
    Connect2Model model_;
    uint32_t num_simulations_;

};

#endif /* TRAINER_H */
