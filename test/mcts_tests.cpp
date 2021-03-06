#include <gtest/gtest.h>
#include <monte_carlo_tree_search.h>

TEST(MCTSTests, NodeConstructorWorks) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  ASSERT_EQ(node.IsExpanded(), false);
  ASSERT_EQ(node.GetValue(), 0);
}

TEST(MCTSTests, NodeCanExpandCorrectly) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<int> state = {0, 0, 0, 0};
  std::vector<float> actionProbs = {0.25, 0.25, 0.25, 0.25};
  node.Expand(state, toPlay, actionProbs);

  ASSERT_EQ(node.IsExpanded(), true);
}

TEST(MCTSTests, CanMaskAndNormalize_AllValid) {
  std::vector<float> actionProbs = {0.25, 0.25, 0.25, 0.25};
  std::vector<int> validMoves = {1, 1, 1, 1};

  MCTS::MaskInvalidMovesAndNormalize(actionProbs, validMoves);

  ASSERT_EQ(actionProbs[0], 0.25);
  ASSERT_EQ(actionProbs[0], 0.25);
  ASSERT_EQ(actionProbs[0], 0.25);
  ASSERT_EQ(actionProbs[0], 0.25);
}

TEST(MCTSTests, CanMaskAndNormalize_AllMasked) {
  std::vector<float> actionProbs = {0.25, 0.25, 0.25, 0.25};
  std::vector<int> validMoves = {0, 0, 0, 0};

  actionProbs = MCTS::MaskInvalidMovesAndNormalize(actionProbs, validMoves);

  ASSERT_EQ(actionProbs[0], 0);
  ASSERT_EQ(actionProbs[1], 0);
  ASSERT_EQ(actionProbs[2], 0);
  ASSERT_EQ(actionProbs[3], 0);
}

TEST(MCTSTests, CanMaskAndNormalize_PartialMask1) {
  std::vector<float> actionProbs = {0.25, 0.25, 0.25, 0.25};
  std::vector<int> validMoves = {1, 1, 0, 0};

  actionProbs = MCTS::MaskInvalidMovesAndNormalize(actionProbs, validMoves);

  ASSERT_EQ(actionProbs[0], 0.5);
  ASSERT_EQ(actionProbs[1], 0.5);
  ASSERT_EQ(actionProbs[2], 0);
  ASSERT_EQ(actionProbs[3], 0);
}

TEST(MCTSTests, CanMaskAndNormalize_PartialMask2) {
  std::vector<float> actionProbs = {0.25, 0.25, 0.25, 0.25};
  std::vector<int> validMoves = {1, 0, 0, 1};

  actionProbs = MCTS::MaskInvalidMovesAndNormalize(actionProbs, validMoves);

  ASSERT_EQ(actionProbs[0], 0.5);
  ASSERT_EQ(actionProbs[1], 0);
  ASSERT_EQ(actionProbs[2], 0);
  ASSERT_EQ(actionProbs[3], 0.5);
}

TEST(MCTSTests, Backup_OneNode_PositiveValue) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node};

  float val = 1.0;
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node.GetValue(), 1.0);
  ASSERT_EQ(node.GetVisitCount(), 1);
}

TEST(MCTSTests, Backup_OneNode_PositiveValue2) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node};

  float val = 0.75;
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node.GetValue(), 0.75);
  ASSERT_EQ(node.GetVisitCount(), 1);
}

TEST(MCTSTests, Backup_OneNode_PositiveValue3) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node};

  float val = 0.75;
  toPlay = -1;  // <- From perspective of opponent
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node.GetValue(), -0.75);
  ASSERT_EQ(node.GetVisitCount(), 1);
}

TEST(MCTSTests, Backup_OneNode_NegativeValue) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node};

  float val = -1.0;
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node.GetValue(), -1.0);
  ASSERT_EQ(node.GetVisitCount(), 1);
}

TEST(MCTSTests, Backup_OneNode_TwoBackupsOneValue) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node};

  float val = 1.0;
  MCTS::Backup(searchPath, val, toPlay);

  // Another backup with the same value
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node.GetValue(), 1.0);
  ASSERT_EQ(node.GetVisitCount(), 2);
}

TEST(MCTSTests, Backup_OneNode_TwoBackupsTwoValues) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node};

  float val = 1.0;
  MCTS::Backup(searchPath, val, toPlay);

  val = -1.0;
  // Another backup with the same value
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node.GetValue(), 0.0);
  ASSERT_EQ(node.GetVisitCount(), 2);
}

TEST(MCTSTests, Backup_TwoNodes_PositiveValue) {
  int prior = 0.5;
  int toPlay = 1;
  int action = 0;
  Node node1(prior, toPlay, action);

  toPlay = -1;
  Node node2(prior, toPlay, action);

  std::vector<Node*> searchPath = {&node1, &node2};

  float val = 1.0;
  toPlay = 1;
  MCTS::Backup(searchPath, val, toPlay);

  ASSERT_EQ(node1.GetValue(), 1.0);
  ASSERT_EQ(node1.GetVisitCount(), 1);

  ASSERT_EQ(node2.GetValue(), -1.0);
  ASSERT_EQ(node2.GetVisitCount(), 1);
}

struct Connect2MockModel : Model {
  std::vector<float> action_probs;
  float value;

  Connect2MockModel(int board_size, int action_size, 
                    std::vector<float> action_probs, float value)
    : Model(board_size, action_size), action_probs(action_probs), value(value) 
    {}

  ActionProbsAndValueTensor forward(const torch::Tensor& input) override {
    throw "forward() is not mocked.";
  }

  ActionProbsAndValue predict(std::vector<int>& board) override {
    return {action_probs, value};
  }
};

Connect2MockModel GetMockModel(std::vector<float> action_probs, float value) {

  int board_size = 4;
  int action_size = 4;

  Connect2MockModel model(board_size, action_size, action_probs, value);
  return model;
}

TEST(MCTSTests, RootWithEqualPriors) {
  int board_size = 4;
  int action_size = 4;
  auto game = Connect2Game();
  std::vector<float> action_probs = {0.26, 0.24, 0.24, 0.26};
  float value = 0.0001;
  auto model = GetMockModel(action_probs, value);
  std::vector<int> state = {0, 0, 0, 0};
  auto mcts = MCTS(game, model);

  auto root = mcts.Run(state, /*to_play=*/1, /*num_simulations=*/50);
  auto best_inner_move = std::max(root->GetChildByAction(1)->GetVisitCount(),
                                  root->GetChildByAction(2)->GetVisitCount());
  auto best_outer_move = std::max(root->GetChildByAction(0)->GetVisitCount(),
                                 root->GetChildByAction(3)->GetVisitCount());

  ASSERT_GT(best_inner_move, best_outer_move);
}

TEST(MCTSTests, MCTSFindsBestMoveWithGoodPriors) {
  int board_size = 4;
  int action_size = 4;
  auto game = Connect2Game();
  std::vector<float> action_probs = {0.3, 0.7, 0.0, 0.0};
  float value = 0.0001;
  auto model = GetMockModel(action_probs, value);
  std::vector<int> state = {0, 0, 1, -1};
  auto mcts = MCTS(game, model);

  auto root = mcts.Run(state, /*to_play=*/1, /*num_simulations=*/25);

  auto pos_0_count = root->GetChildByAction(0)->GetVisitCount();
  auto pos_1_count = root->GetChildByAction(1)->GetVisitCount();

  ASSERT_GT(pos_1_count, pos_0_count);
}

TEST(MCTSTests, MCTSFindsBestMoveWithBadPriors) {
  int board_size = 4;
  int action_size = 4;
  auto game = Connect2Game();
  std::vector<float> action_probs = {0.7, 0.3, 0.0, 0.0};
  float value = 0.0001;
  auto model = GetMockModel(action_probs, value);
  std::vector<int> state = {0, 0, 1, -1};
  auto mcts = MCTS(game, model);

  auto root = mcts.Run(state, /*to_play=*/1, /*num_simulations=*/25);

  auto pos_0_count = root->GetChildByAction(0)->GetVisitCount();
  auto pos_1_count = root->GetChildByAction(1)->GetVisitCount();

  ASSERT_GT(pos_1_count, pos_0_count);
}

TEST(MCTSTests, MCTSFindsBestMoveWithEqualPriors) {
  int board_size = 4;
  int action_size = 4;
  auto game = Connect2Game();
  std::vector<float> action_probs = {0.51, 0.49, 0.0, 0.0};
  float value = 0.0001;
  auto model = GetMockModel(action_probs, value);
  std::vector<int> state = {0, 0, 1, -1};
  auto mcts = MCTS(game, model);

  auto root = mcts.Run(state, /*to_play=*/1, /*num_simulations=*/25);

  auto pos_0_count = root->GetChildByAction(0)->GetVisitCount();
  auto pos_1_count = root->GetChildByAction(1)->GetVisitCount();

  ASSERT_GT(pos_1_count, pos_0_count);
}

TEST(MCTSTests, MCTSFindsBestMoveWithEqualPriors2) {
  int board_size = 4;
  int action_size = 4;
  auto game = Connect2Game();
  std::vector<float> action_probs = {0.1, 0.3, 0.3, 0.3};
  float value = 0.0001;
  auto model = GetMockModel(action_probs, value);
  std::vector<int> state = {-1, 0, 0, 0};
  auto mcts = MCTS(game, model);

  auto root = mcts.Run(state, /*to_play=*/1, /*num_simulations=*/100);

  auto pos_1_count = root->GetChildByAction(1)->GetVisitCount();
  auto pos_2_count = root->GetChildByAction(2)->GetVisitCount();
  auto pos_3_count = root->GetChildByAction(3)->GetVisitCount();

  ASSERT_GT(pos_1_count, pos_2_count);
  ASSERT_GT(pos_1_count, pos_3_count);
}

TEST(MCTSTests, MCTSBlocksPlayer) {
  int board_size = 4;
  int action_size = 4;
  auto game = Connect2Game();
  std::vector<float> action_probs = {0.25, 0.25, 0.25, 0.25};
  float value = 0.0001;
  auto model = GetMockModel(action_probs, value);
  std::vector<int> state = {0, 0, -1, 0};
  auto mcts = MCTS(game, model);

  auto root = mcts.Run(state, /*to_play=*/-1, /*num_simulations=*/100);

  auto pos_0_count = root->GetChildByAction(0)->GetVisitCount();
  auto pos_1_count = root->GetChildByAction(1)->GetVisitCount();
  auto pos_3_count = root->GetChildByAction(3)->GetVisitCount();

  ASSERT_LT(pos_0_count, pos_1_count);
  ASSERT_LT(pos_0_count, pos_3_count);
}