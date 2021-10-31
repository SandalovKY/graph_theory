#pragma once
#include "graph.hpp"

template<typename Graph>
struct Algorithm;

template<>
struct Algorithm<my::BitAdjacencyMatrix>
{
    static std::vector<std::vector<uint16_t> > coloring(const my::BitAdjacencyMatrix& adjMatr)
    {
        std::vector<std::vector<uint16_t>> resColors{};
        resColors.emplace_back(std::vector<uint16_t>{1, 1, 1, 1});

        return resColors;
    }
};