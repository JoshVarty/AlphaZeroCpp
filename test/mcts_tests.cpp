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
