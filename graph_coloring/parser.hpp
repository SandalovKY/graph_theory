#pragma once

#include "graph.hpp"
#include "bitset.hpp"

#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "boost/dynamic_bitset/dynamic_bitset.hpp"

class Parser
{
public:
    using ReorderingMap = std::map<size_t, size_t>;
    using AdjList = std::vector<std::pair<size_t, std::set<size_t> > >;

    Parser(char const* filename);

    template <typename ListType>
    BitAdjacencyMatrix<ListType>
    adjList2adjMatr(ReorderingMap* map2Reorder = nullptr)
    {
        BitAdjacencyMatrix<ListType> adjMatrRet{ static_cast<typename BitAdjacencyMatrix<ListType>::m_vertex_num_type>(m_vertexNum),
            static_cast<typename BitAdjacencyMatrix<ListType>::m_edges_num_type>(m_edgesNum) };
        
        if (map2Reorder != nullptr)
        {
            for (auto& vertex: m_list)
            {
                for(const auto& adjacent: vertex.second)
                {
                    adjMatrRet[(*map2Reorder)[vertex.first]].set((*map2Reorder)[adjacent]);
                }
            }
        }
        else
        {
            for (const auto& vertex: m_list)
            {
                for(const auto& adjacent: vertex.second)
                {
                    adjMatrRet[vertex.first].set(adjacent);
                }
            }
        }
        return adjMatrRet;
    }

    typename std::map<size_t, myBitset<>>
    adjList2adjMatrMap(ReorderingMap* map2Reorder = nullptr);

    typename std::map<size_t, boost::dynamic_bitset<>>
    adjList2adjMatrMapBoost(ReorderingMap* map2Reorder = nullptr);

    ReorderingMap getSimpleMaxCliqueReordering();
    ReorderingMap getCoreNumsMaxCliqueReordering();
    std::set<size_t> getDefaultOrder(ReorderingMap& reorderingMap, std::set<size_t>& srcSet);
    bool provedClique(const std::set<size_t>& clique);
private:
    AdjList m_list{};
    size_t m_vertexNum{};
    size_t m_edgesNum{};
};
