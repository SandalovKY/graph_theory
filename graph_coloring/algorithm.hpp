#pragma once
#include "graph.hpp"
#include "ant_colony.hpp"
#include <iostream>
// #include <stack>

template<typename Graph>
struct Algorithm;

template<typename NeighboursList>
struct Algorithm<my::BitAdjacencyMatrix<NeighboursList> >
{
    static std::vector<std::vector<uint16_t> > coloring(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::vector<uint16_t>> resColors{};
        size_t color_pos{ 0 };
        size_t dimSize = adjMatr.getMatrDimSize();
        // std::cout << "Dim size: " << dimSize << std::endl;
        std::vector<bool> isVertColored(dimSize, false);

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].set(ind);
            ~adjMatr[ind];
        }

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            // std::cout << "Ind: " << ind << std::endl;
            if (!isVertColored[ind])
            {
                resColors.push_back(std::vector<uint16_t>());
                resColors[color_pos].push_back(ind);
                auto& bitSet = adjMatr.getLine(ind);
                // std::cout << "Bitset:\n" << bitSet << std::endl;
                size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
                // std::cout << firstSetBitPos << std::endl;
                // std::cout << "Finding not adjunced verts: " << firstSetBitPos << std::endl;
                while (firstSetBitPos != dimSize)
                {
                    // std::cout << "Inverted bitset:\n" << bitSet << std::endl;
                    if (!isVertColored[firstSetBitPos])
                    {
                        bitSet &= adjMatr.getLine(firstSetBitPos);
                        resColors[color_pos].push_back(firstSetBitPos);
                    }
                    else
                    {
                        bitSet.unset(firstSetBitPos);
                        // std::cout << "Was undet bit: " << firstSetBitPos << std::endl;
                    }
                    firstSetBitPos = bitSet.getFirstNonZeroPosition();
                }
                for (const auto& vert : resColors[color_pos])
                {
                    isVertColored[vert] = true;
                }
                ++color_pos;
            }
        }
        return resColors;
    }

    static std::vector<std::set<size_t>> coloring1(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::set<size_t>> possibleCliques{};
        size_t dimSize = adjMatr.getMatrDimSize();
        std::vector<bool> isVertVisited(dimSize, false);

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].unset(ind);
        }

        // std::cout << "Algorithm started\n";

        for(size_t ind = 0; ind < dimSize; ++ind)
        {
            std::cout << "Index: " << ind << std::endl;
            if (isVertVisited[ind]) continue;
            std::vector<bool> isVertVisitedLocally(dimSize, false);
            isVertVisited[ind] = true;
            auto bitset = adjMatr.getLine(ind);
            size_t adjVert = bitset.getFirstNonZeroPosition();
            while (adjVert != dimSize)
            {
                // std::cout << "AdjVert: " << adjVert << std::endl;
                if (!isVertVisitedLocally[adjVert])
                {
                    size_t localAdjVert = adjVert;
                    auto localBitset = adjMatr[ind];
                    std::set<size_t> cliqueSet{ ind };
                    while (localAdjVert != dimSize)
                    {
                        // std::cout << "LocalAdjVert: " << localAdjVert << std::endl;
                        // std::cout << "LocalBitset: " << localBitset << std::endl;
                        isVertVisited[localAdjVert] = true;
                        isVertVisitedLocally[localAdjVert] = true;
                        cliqueSet.insert(localAdjVert);
                        auto& adjVertBitset = adjMatr.getLine(localAdjVert);
                        localBitset &= adjVertBitset;
                        localAdjVert = localBitset.getFirstNonZeroPosition();
                    }
                    if (cliqueSet.size() > 2) possibleCliques.emplace_back(std::move(cliqueSet));
                }
                bitset.unset(adjVert);
                adjVert = bitset.getFirstNonZeroPosition();
            }
        }
        return possibleCliques;
    }
};