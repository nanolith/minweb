/**
 * \file minweb/graph.h
 *
 * \brief Graph utilities.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#pragma once

/** C++ version check. */
#if !defined(__cplusplus) || __cplusplus < 201402L
# error This file requires C++14 or greater.
#endif

#include <list>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>

namespace minweb {

    /**
     * \brief The cycle_error is an exception that occurs when a cycle is found
     * in a directed graph when an acyclic graph is expected.
     */
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

        /**
         * \brief Add a node to the graph.
         *
         * \param node      The node to add to the graph.
         */
        void add_node(int node);

        /**
         * \brief Add an edge to the graph.
         *
         * \param node      The "from" node that this edge is connecting.
         * \param toNode    The "to" node that this edge is connecting.
         */
        void add_edge(int node, int toNode);

        /**
         * \brief Sort the nodes of this graph in dependency order, so that the
         * output list contains a sequence in which node dependencies are
         * visited before nodes with these dependencies, using a quick and dirty
         * variation of Kahn's Algorithm.
         *
         * \param output_list       The list to receive the nodes to visit in
         *                          order.
         *
         * \throws cycle_error if a cycle is found in the graph.
         */
        void topological_sort(std::list<int>& output_list) const;

    private:
        std::map<int, std::shared_ptr<std::set<int>>> nodes;
    };

} /* namespace minweb */
