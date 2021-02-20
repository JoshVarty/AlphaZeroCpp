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

struct TrainerOptions {
  uint32_t batch_size;
  uint32_t num_episodes;
  uint32_t num_epochs;
  uint32_t num_simulations;
  uint32_t training_iterations;
};

class Trainer {
  public:
    Trainer(Connect2Game game, Connect2Model model, TrainerOptions options) : 
      game_(game), 
      model_(model),
      options_(options) {}

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
    TrainerOptions options_;
};

#endif /* TRAINER_H */