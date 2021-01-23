#ifndef MODEL_H
#define MODEL_H

#include <torch/torch.h>

// During training, we need to keep a copy of the original tensors
// so we can create a loss and backprop through everything.
struct ActionProbsAndValueTensor {
  torch::Tensor action_probs;
  torch::Tensor value;
};

// During prediction, we simply want the raw numerical values for
// actionProbs and our value.
struct ActionProbsAndValue {
  std::vector<float> action_probs;
  float value;
};

struct Model {
  Model(int board_size, int action_size)
      : board_size(board_size), action_size(action_size) {}

  int board_size;
  int action_size;

  virtual ActionProbsAndValueTensor forward(const torch::Tensor& input) = 0;
  virtual ActionProbsAndValue predict(std::vector<int>& board) = 0;
};

struct Connect2Model : torch::nn::Module, Model {
  Connect2Model(int board_size, int action_size, torch::Device device)
      : Model(board_size, action_size),
        device(device),
        fc1(register_module("fc1", torch::nn::Linear(board_size, 16))),
        fc2(register_module("fc2", torch::nn::Linear(16, 16))),
        action_head(
            register_module("action_head", torch::nn::Linear(16, action_size))),
        value_head(register_module("value_head", torch::nn::Linear(16, 1))) {
    this->to(device);
  }

  ActionProbsAndValueTensor forward(const torch::Tensor& input) override {
    auto x = torch::relu(fc1(input));
    x = torch::relu(fc2(x));

    auto action_logits = action_head(x);
    auto value_logit = value_head(x);

    auto action_probs = torch::softmax(action_logits, 1);
    auto value = torch::tanh(value_logit);

    return {action_probs, value};
  }

  ActionProbsAndValue predict(std::vector<int>& board) override {
    this->eval();

    auto opts = torch::TensorOptions().dtype(torch::kInt32);
    auto input =
        torch::from_blob(board.data(), board.size(), opts).to(torch::kFloat32);
    input = input.view({1, board_size});

    torch::NoGradGuard guard;

    ActionProbsAndValueTensor result = this->forward(input);

    // Unpack values from tensors into primitive types
    auto action_probs_tensor = result.action_probs.cpu();
    auto value = result.value.cpu().item<float>();
    std::vector<float> action_probs(
        action_probs_tensor.data_ptr<float>(),
        action_probs_tensor.data_ptr<float>() + action_probs_tensor.numel());

    return {action_probs, value};
  }

  torch::Device device;

  torch::nn::Linear fc1;
  torch::nn::Linear fc2;
  torch::nn::Linear action_head;
  torch::nn::Linear value_head;
};

#endif /* MODEL_H */
