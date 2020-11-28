#ifndef MODEL_H
#define MODEL_H

#include <torch/torch.h>

// During training, we need to keep a copy of the original tensors
// so we can create a loss and backprop through everything.
struct ActionProbsAndValueTensor {
    torch::Tensor actionProbs;
    torch::Tensor value;
};

// During prediction, we simply want the raw numerical values for 
// actionProbs and our value.
struct ActionProbsAndValue
{
    std::vector<float> actionProbs;
    float value;
};


struct Connect2Model : torch::nn::Module {

    Connect2Model(int boardSize, int actionSize, torch::Device device) : 
    _boardSize(boardSize),
    _actionSize(actionSize),
    _device(device),
    _fc1(register_module("_fc1", torch::nn::Linear(boardSize, 16))),
    _fc2(register_module("_fc2", torch::nn::Linear(16, 16))),
    _actionHead(register_module("_actionHead", torch::nn::Linear(16, actionSize))),
    _valueHead(register_module("_valueHead", torch::nn::Linear(16, 1)))
    {
        this->to(device);
    }

    ActionProbsAndValueTensor forward(torch::Tensor input) {
        auto x = torch::relu(_fc1(input));
        x = torch::relu(_fc2(x));

        auto action_logits = _actionHead(x);
        auto value_logit = _valueHead(x);

        auto actionProbs = torch::softmax(action_logits, 1);
        auto value = torch::tanh(value_logit);

        ActionProbsAndValueTensor returnVal;
        returnVal.actionProbs = actionProbs;
        returnVal.value = value;

        return returnVal;
    }

    ActionProbsAndValue predict(std::vector<int> board) {
        this->eval();

        auto opts = torch::TensorOptions().dtype(torch::kInt32);
        auto input = torch::from_blob(board.data(), board.size(), opts).to(torch::kFloat32);
        input = input.view({1, _boardSize});

        torch::NoGradGuard guard;

        ActionProbsAndValueTensor result = this->forward(input);

        // Unpack values from tensors into primitive types
        auto actionProbsTensor = result.actionProbs.cpu();
        auto value = result.value.cpu().item<float>();
        std::vector<float> actionProbs(actionProbsTensor.data_ptr<float>(), actionProbsTensor.data_ptr<float>() + actionProbsTensor.numel());

        ActionProbsAndValue apv = { actionProbs, value };
        return apv;
    }

    int _boardSize;
    int _actionSize;
    torch::Device _device;

    torch::nn::Linear _fc1;
    torch::nn::Linear _fc2;
    torch::nn::Linear _actionHead;
    torch::nn::Linear _valueHead;
};

#endif /* MODEL_H */