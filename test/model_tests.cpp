#include <gtest/gtest.h>
#include <model.h>


TEST(ModelTests, EnsureWeCanCreateModel) {
    Connect2Model model(4, 4, torch::kCPU);
}
