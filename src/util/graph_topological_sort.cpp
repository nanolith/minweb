/**
 * \file util/graph_topological_sort.cpp
 *
 * \brief Perform a topological sort on the graph, returning nodes sorted in
 * order of dependency.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/graph.h>

using namespace minweb;
using namespace std;

/**
 * Quick and dirty variation of Kahn's Algorithm for topological sorting.
 */
void minweb::graph::topological_sort(std::list<int>& output_list) const
{
    map<int, shared_ptr<set<int>>> sort_nodes;

    /* copy nodes to sort_nodes. */
    for (auto i : nodes)
    {
        sort_nodes.insert(
            make_pair(
                i.first,
                make_shared<set<int>>(*i.second)));
    }

    while (sort_nodes.size() > 0)
    {
        bool found = false;

        for (auto i : sort_nodes)
        {
            /* if this node has no dependencies...*/
            if (i.second->size() == 0)
            {
                /* remove this node as a dependency for any other node. */
                for (auto j : sort_nodes)
                {
                    j.second->erase(i.first);
                }

                /* add this node to the output list. */
                output_list.push_back(i.first);

                /* remove this node from the sort_nodes map. */
                sort_nodes.erase(i.first);

                /* a node was found. */
                found = true;

                /* break out of this loop, because it was invalidated by the
                 * erasure. */
                break;
            }
        }

        /* if no node was found, then we have a cycle. */
        if (!found)
        {
            throw cycle_error();
        }
    }
}
