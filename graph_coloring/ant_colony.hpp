#pragma once

#include "graph.hpp"

#include <vector>
#include <utility>
#include <map>
#include <set>
#include <boost/graph/graph_traits.hpp>


class AntColony
{
private:
    using vert_descr_t = boost::graph_traits<ColoredGraph>::vertex_descriptor;
    using edge_descr_t = boost::graph_traits<ColoredGraph>::edge_descriptor;
    using vert_iter_t = boost::graph_traits<ColoredGraph>::vertex_iterator;
    using adj_iter_t = boost::graph_traits<ColoredGraph>::adjacency_iterator;
    using traits_t = std::map<std::pair<vert_descr_t, vert_descr_t>, size_t>;
    using color_groups_t = std::vector<std::set<vert_descr_t>>;
    using tabu_list_t = std::map<vert_descr_t, size_t>;


    color_groups_t m_groups{68};
    traits_t m_globalTraits{};
    tabu_list_t m_tabuMoves{};
    ColoredGraph& m_graphRef;

    size_t m_numAnts{};

    class Ant
    {
        using vert_descr_t = boost::graph_traits<ColoredGraph>::vertex_descriptor;

        size_t m_groups_num{68};
        int64_t antID{};
        std::set<vert_descr_t> workingSet{};
    public:
        Ant(int64_t index) : antID(index) {}
        void setAntIndex(int64_t index)
        {
            antID = index;
        }
        void addToWorkingSet(vert_descr_t descr)
        {
            workingSet.insert(descr);
        }
        traits_t doWork(tabu_list_t& tabuMoves, traits_t& globalTraits, ColoredGraph& graph, color_groups_t& colorGroups);
    };

    std::vector<Ant> m_colony{};
public:
    AntColony(ColoredGraph& graph, int antsCount = 10);
    void doWork(size_t maxIter);
};
