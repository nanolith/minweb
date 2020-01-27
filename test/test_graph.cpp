/**
 * \file test/test_graph.cpp
 *
 * \brief Unit tests for graph.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <iostream>
#include <minunit/minunit.h>
#include <minweb/graph.h>

using namespace minweb;
using namespace std;

TEST_SUITE(graph);

/**
 * A topological sort of a single node with no edges returns that node.
 */
TEST(single_node)
{
    graph g;
    list<int> sorted_nodes;

    g.add_node(0);
    g.topological_sort(sorted_nodes);

    TEST_EXPECT(1 == sorted_nodes.size());
    TEST_EXPECT(0 == sorted_nodes.front());
}

/**
 * A topological sort of two nodes, 1 and 2, where 1 depends on 2.
 */
TEST(two_node_dependency)
{
    graph g;
    list<int> sorted_nodes;

    g.add_edge(1, 2);
    g.topological_sort(sorted_nodes);

    TEST_ASSERT(2 == sorted_nodes.size());
    TEST_EXPECT(2 == sorted_nodes.front());
    TEST_EXPECT(1 == sorted_nodes.back());
}

/**
 * More complex scenario...
 */
TEST(three_node_dependency)
{
    graph g;
    list<int> sorted_nodes;

    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.topological_sort(sorted_nodes);

    TEST_ASSERT(3 == sorted_nodes.size());

    auto f = sorted_nodes.begin();
    TEST_ASSERT(f != sorted_nodes.end());
    TEST_EXPECT(3 == *f);
    ++f;
    TEST_ASSERT(f != sorted_nodes.end());
    TEST_EXPECT(2 == *f);
    ++f;
    TEST_ASSERT(f != sorted_nodes.end());
    TEST_EXPECT(1 == *f);
    ++f;
    TEST_ASSERT(f == sorted_nodes.end());
}

/**
 * Cycle detection.
 */
TEST(cycle)
{
    graph g;
    list<int> sorted_nodes;

    g.add_edge(1, 2);
    g.add_edge(2, 3);
    /* 3 depends on 1, which indirectly depends on 3, thus cycle. */
    g.add_edge(3, 1);

    try
    {
        g.topological_sort(sorted_nodes);
        TEST_FAILURE();
    }
    catch (cycle_error& e)
    {
        TEST_SUCCESS();
    }
}
