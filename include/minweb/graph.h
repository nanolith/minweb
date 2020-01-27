/**
 * \file minweb/graph.h
 *
 * \brief Graph utilities.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#ifndef  MINWEB_GRAPH_HEADER_GUARD
# define MINWEB_GRAPH_HEADER_GUARD

/** C++ version check. */
#if !defined(__cplusplus) || __cplusplus < 201402L
# error This file requires C++14 or greater.
#endif

#include <list>
#include <map>
#include <memory>
#include <set>

namespace minweb {

    class cycle_error : public std::runtime_error
    {
    public:
        cycle_error()
            : runtime_error("cycle detected.") { }
    };

    /**
     * \brief A graph of nodes and edges.
     */
    class graph
    {
    public:
        void add_node(int node);
        void add_edge(int node, int toNode);
        void topological_sort(std::list<int>& output_list) const;

    private:
        std::map<int, std::shared_ptr<std::set<int>>> nodes;
    };

} /* namespace minweb */

#endif /*MINWEB_GRAPH_HEADER_GUARD*/
