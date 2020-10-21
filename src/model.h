#include <torch/torch.h>

struct ActionValuePair {
    torch::Tensor action;
    torch::Tensor value;
};

struct PredictPair
{
    std::vector<double> probs;
    double value;
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

    ActionValuePair forward(torch::Tensor input) {
        auto x = torch::relu(_fc1(input));
        x = torch::relu(_fc2(x));

        auto action_logits = _actionHead(x);
        auto value_logit = _valueHead(x);

        auto action = torch::softmax(action_logits, 1);
        auto value = torch::tanh(value_logit);

        ActionValuePair returnVal;
        returnVal.action = action;
        returnVal.value = value;

        return returnVal;
    }

    int predict(std::vector<int> board) {
        this->eval();

        auto opts = torch::TensorOptions().dtype(torch::kInt32);
        auto input = torch::from_blob(board.data(), board.size(), opts).to(torch::kFloat32);
        input = input.view({1, _boardSize});

        torch::NoGradGuard guard;

        auto result = this->forward(input);

        // TODO: Get data from GPU, return as vanilla vector<int> and int

        return 0;
    }

    int _boardSize;
    int _actionSize;
    torch::Device _device;

    torch::nn::Linear _fc1;
    torch::nn::Linear _fc2;
    torch::nn::Linear _actionHead;
    torch::nn::Linear _valueHead;
};