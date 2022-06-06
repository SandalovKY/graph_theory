#pragma once

#include "graph.hpp"
#include "ant_colony.hpp"

#include <iostream>
#include <set>
#include <unordered_map>


template<typename Graph>
struct Algorithm
{
    size_t m_maxIter = 1;
    AntColony m_colony;
    Algorithm(Graph& graph, size_t numAnts = 10, size_t maxIter = 1500) : m_colony(graph, numAnts), m_maxIter(maxIter) {}

    void coloring()
    {
        m_colony.doWork(m_maxIter);
    }
};

template<typename NeighboursList>
struct Algorithm<BitAdjacencyMatrix<NeighboursList>>
{
    static std::vector<std::vector<uint16_t>> coloring(BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::vector<uint16_t>> resColors{};
        size_t color_pos{ 0 };
        size_t dimSize = adjMatr.getMatrDimSize();
        std::vector<bool> isVertColored(dimSize, false);

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].set(ind);
            ~adjMatr[ind];
        }

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            if (!isVertColored[ind])
            {
                resColors.push_back(std::vector<uint16_t>());
                resColors[color_pos].push_back(ind);
                auto& bitSet = adjMatr.getLine(ind);
                size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
                while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
                {
                    if (!isVertColored[firstSetBitPos])
                    {
                        bitSet &= adjMatr.getLine(firstSetBitPos);
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

    static std::vector<std::vector<size_t>> maxCliqueFinding(BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::vector<size_t>> allMaxCliques{};
        size_t dimSize = adjMatr.getMatrDimSize();

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].unset(ind);
        }

        for(size_t ind = 0; ind < dimSize; ++ind)
        {
            bool isFinished{ false };
            size_t maxCliqueSize{ 0 };
            while (!isFinished)
            {
                auto pathBitset = adjMatr.getLine(ind);
                auto curBitset = pathBitset;
                std::vector<size_t> cliqueVector{ ind };

                size_t adjVert = pathBitset.getFirstNonZeroPosition(); 
                size_t iter {0};
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
                    isFinished = iter == 0 && numNbr <= maxCliqueSize;
                    cliqueVector.push_back(nextVert);
                    pathBitset &= adjMatr.getLine(nextVert);
                    adjVert = pathBitset.getFirstNonZeroPosition();
                    curBitset = pathBitset;
                    ++iter;
                }
                if (cliqueVector.size() > maxCliqueSize)
                {
                    allMaxCliques.emplace_back(std::move(cliqueVector));
                    maxCliqueSize = cliqueVector.size();
                }
                if (cliqueVector.size() > 2)
                {
                    size_t initVert = cliqueVector[1];
                    for (size_t ind = 2; ind < cliqueVector.size(); ++ind)
                    {
                        adjMatr.getLine(cliqueVector[ind]).unset(initVert);
                        adjMatr.getLine(initVert).unset(cliqueVector[ind]);
                    }
                }
                else
                {
                    isFinished = true;
                    allMaxCliques.emplace_back(std::move(cliqueVector));
                }
            }
        }
        return allMaxCliques;
    }
};
