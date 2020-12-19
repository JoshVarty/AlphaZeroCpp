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
    node.Expand(state, toPlay,  actionProbs);

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

TEST(MCTSTests, Backup_OneNode) {
    int prior = 0.5;
    int toPlay = 1;
    int action = 0;
    Node node(prior, toPlay, action);

    std::vector<Node*> searchPath = { &node };

    int val = 1;
    MCTS::Backup(searchPath, val, toPlay);

    ASSERT_EQ(searchPath[0]->GetValue(), 1.0);
    ASSERT_EQ(searchPath[0]->GetVisitCount(), 1);
}