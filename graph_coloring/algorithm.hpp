#pragma once
#include "graph.hpp"
#include "ant_colony.hpp"
#include <iostream>

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

    static std::vector<std::vector<uint16_t> > coloring1(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::vector<uint16_t>> cliques{};
        size_t numCliques{ 0 };
        size_t dimSize = adjMatr.getMatrDimSize();
        // std::cout << "Dim size: " << dimSize << std::endl;
        std::vector<bool> isVertVisited(dimSize, false);
        for(size_t ind = 0; ind < dimSize; ++ind)
        {

        }
    }
};