#include <gtest/gtest.h>
#include <model.h>


TEST(ModelTests, EnsureWeCanCreateModel) {
    Connect2Model model(4, 4, torch::kCPU);

    auto options = torch::TensorOptions().dtype(torch::kFloat32);
    auto x = torch::zeros({1, 4}, options);
    auto sz = x.sizes();
    
    auto result = model.forward(x);

}
