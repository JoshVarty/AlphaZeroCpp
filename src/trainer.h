#ifndef TRAINER_H
#define TRAINER_H

#include "game.h"
#include "model.h"
#include "monte_carlo_tree_search.h"

#include <experimental/filesystem>
#include <torch/torch.h>
#include <sys/types.h>
#include <sys/stat.h>

struct Example {
  std::vector<int> canonical_board;
  int current_player;
  std::vector<float> action_probs;
  int reward;
};

class Trainer {
  public:
    Trainer(Connect2Game game, Connect2Model model, uint32_t num_simulations,
            uint32_t training_iterations, uint32_t number_of_episodes) : 
      game_(game), 
      model_(model), 
      num_simulations_(num_simulations),
      training_iterations_(training_iterations), 
      number_of_episodes_(number_of_episodes) {}

    std::vector<Example> ExecuteEpisode();
    void Learn();
    void Train(const std::vector<Example>& examples);
    torch::Tensor GetProbabilityLoss(torch::Tensor targets,
                                     torch::Tensor outputs);
    torch::Tensor GetValueLoss(torch::Tensor targets,
                               torch::Tensor outputs);
    void SaveCheckpoint(std::string folder, std::string filename);

  private:
    Connect2Game game_;
    Connect2Model model_;
    uint32_t num_simulations_;
    uint32_t training_iterations_;
    uint32_t number_of_episodes_;
};

#endif /* TRAINER_H */