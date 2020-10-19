#include <iostream>
#include <torch/torch.h>
#include <game.h>


int main() {
    auto tensor = torch::eye(3);
    std::cout << tensor << std::endl;
}