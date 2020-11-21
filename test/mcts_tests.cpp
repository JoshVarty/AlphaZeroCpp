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
