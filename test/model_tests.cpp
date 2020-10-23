#include <gtest/gtest.h>
#include <model.h>


TEST(ModelTests, EnsureWeCanCreateModel) {
    Connect2Model model(4, 4, torch::kCPU);

    ASSERT_EQ(model._boardSize, 4);
    ASSERT_EQ(model._actionSize, 4);
    ASSERT_EQ(model._device, torch::kCPU);
}

TEST(ModelTests, EnsureWeCanPassZeroThroughNetwork) {
    Connect2Model model(4, 4, torch::kCPU);

    auto options = torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU);
    auto input = torch::zeros({1, 4}, options);
    auto output = model.forward(input);

    ASSERT_EQ(input.size(0), output.action.size(0));
    ASSERT_EQ(input.size(1), output.action.size(1));
    ASSERT_EQ(output.value.size(0), 1);
}
