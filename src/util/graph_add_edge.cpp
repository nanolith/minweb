/**
 * \file util/graph_add_edge.cpp
 *
 * \brief Add an edge to the graph.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/graph.h>

using namespace minweb;
using namespace std;

void minweb::graph::add_edge(int node, int toNode)
{
    /* does node exist? */
    auto f = nodes.find(node);
    if (f == nodes.end())
    {
        add_node(node);
        f = nodes.find(node);
    }

    /* add toNode to the set of edges for node. */
    f->second->insert(toNode);

    /* does toNode exist? */
    f = nodes.find(toNode);
    if (f == nodes.end())
    {
        add_node(toNode);
    }
}
