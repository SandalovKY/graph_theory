#pragma once
#include "graph.hpp"
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
        std::vector<bool> isVertColored(dimSize);
        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            if (!isVertColored[ind])
            {
                resColors.push_back(std::vector<uint16_t>());
                resColors[color_pos].push_back(ind);
                auto& bitSet = adjMatr.getMatrix(ind);
                bitSet = ~bitSet;
                size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
                while (firstSetBitPos != dimSize)
                {
                    if (!isVertColored[firstSetBitPos])
                    {
                        bitSet &= ~adjMatr.getMatrix(firstSetBitPos);
                        resColors[color_pos].push_back(firstSetBitPos);
                    }
                    else
                    {
                        bitSet.unset(firstSetBitPos);
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
};