#include <iostream>
#include <torch/torch.h>
#include "game.h"


int main() {

    torch::Tensor ten = torch::rand({12, 12}, torch::TensorOptions(torch::kCPU).dtype(at::kFloat)); 
    std::vector<float> v(ten.data_ptr<float>(), ten.data_ptr<float>() + ten.numel());
    for (auto a : v) std::cout << a << std::endl;
}