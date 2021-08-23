/**
 * \file util/graph_add_node.cpp
 *
 * \brief Add a node to the graph.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <minweb/graph.h>

using namespace minweb;
using namespace std;

/**
 * \brief Add a node to the graph.
 *
 * \param node      The node to add to the graph.
 */
void minweb::graph::add_node(int node)
{
    nodes.insert(make_pair(node, make_shared<set<int>>()));
}
