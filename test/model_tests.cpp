#include <gtest/gtest.h>
#include <model.h>

TEST(ModelTests, EnsureWeCanCreateModel) {
  Connect2Model model(4, 4, torch::kCPU);

  ASSERT_EQ(model.board_size, 4);
  ASSERT_EQ(model.action_size, 4);
  ASSERT_EQ(model.device, torch::kCPU);
}

TEST(ModelTests, EnsureWeCanPassZeroThroughNetwork) {
  Connect2Model model(4, 4, torch::kCPU);

  auto options =
      torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU);
  auto input = torch::zeros({1, 4}, options);
  auto output = model.forward(input);

  ASSERT_EQ(input.size(0), output.action_probs.size(0));
  ASSERT_EQ(input.size(1), output.action_probs.size(1));
  ASSERT_EQ(output.value.size(0), 1);
}

TEST(ModelTests, EnsureWeCanPredictOnNetwork) {
  Connect2Model model(4, 4, torch::kCPU);

  auto options =
      torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU);
  std::vector<int> input = {0, 0, 0, 0};
  auto output = model.predict(input);

  // Make sure the input and output shapes match
  ASSERT_EQ(input.size(), output.action_probs.size());
}
