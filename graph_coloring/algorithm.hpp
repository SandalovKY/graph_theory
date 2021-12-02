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

    static std::vector<std::set<size_t>> maxCliqueFinding(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::set<size_t>> allMaxCliques{};
        size_t dimSize = adjMatr.getMatrDimSize();

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].unset(ind);
        }

        for(size_t ind = 0; ind < dimSize; ++ind)
        {
            // Пул вершин, из которых набираем максимальную клику
            auto pathBitset = adjMatr.getLine(ind);
            auto curBitset = pathBitset;
            std::set<size_t> cliqueSet{ ind };

            // Проверка на то, есть ли у нашей вершины вообще соседи
            size_t adjVert = pathBitset.getFirstNonZeroPosition();

            while (adjVert < dimSize)
            {
                size_t nextVert{ adjVert };
                curBitset.unset(adjVert);
                size_t numNbr{ countSetBits(adjMatr.getLine(adjVert) & pathBitset) };
                size_t otherVert{ static_cast<size_t>(curBitset.getFirstNonZeroPosition()) };
                while (otherVert < dimSize)
                {
                    size_t numNbrForOtherVert = countSetBits(adjMatr.getLine(otherVert) & pathBitset);
                    if (numNbrForOtherVert > numNbr)
                    {
                        numNbr = numNbrForOtherVert;
                        nextVert = otherVert;
                    }
                    curBitset.unset(otherVert);
                    otherVert = curBitset.getFirstNonZeroPosition();
                }
                cliqueSet.insert(nextVert);
                pathBitset &= adjMatr.getLine(nextVert);
                adjVert = pathBitset.getFirstNonZeroPosition();
                curBitset = pathBitset;
            }
            if (cliqueSet.size() > 2) allMaxCliques.emplace_back(std::move(cliqueSet));
        }
        return allMaxCliques;
    }
};