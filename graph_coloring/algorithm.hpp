#pragma once
#include "graph.hpp"
#include "ant_colony.hpp"
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>

template<typename Graph>
struct Algorithm
{
    size_t m_maxIter = 1;
    my::AntColony m_colony;
    Algorithm(Graph& graph, size_t numAnts = 10, size_t maxIter = 1500) : m_colony(graph, numAnts), m_maxIter(maxIter) {}

    void coloring()
    {
        m_colony.doWork(m_maxIter);
    }
};

template<typename NeighboursList>
struct Algorithm<my::BitAdjacencyMatrix<NeighboursList>>
{
    static std::vector<std::vector<uint16_t>> coloring(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
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
                while (firstSetBitPos != dimSize)
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

    static size_t maxCliqueFinding1(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        size_t max_clique_size{ 0 };
        size_t dimSize = adjMatr.getMatrDimSize();

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].unset(ind);
        }

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            auto curBitset = adjMatr.getLine(ind);

            std::map<size_t, std::vector<std::pair<size_t, NeighboursList> > > nbrs{};

            size_t adjVert = curBitset.getFirstNonZeroPosition();
            while (adjVert < dimSize)
            {
                auto adjBitset = adjMatr.getLine(adjVert);
                adjBitset &= adjMatr.getLine(ind);
                size_t numNbrVert = countSetBits(adjBitset) + 2;
                adjBitset.set(adjVert);

                auto& nbrsVector = nbrs[numNbrVert];

                auto it = std::find_if(nbrsVector.begin(), nbrsVector.end(), [&adjBitset](std::pair<size_t, NeighboursList>& p){
                    return p.second == adjBitset;
                });
                if (it != nbrsVector.end())
                {
                    it->first += 1;
                }
                else
                {
                    nbrsVector.emplace_back(std::make_pair(1, adjBitset));
                }

                curBitset.unset(adjVert);
                adjVert = curBitset.getFirstNonZeroPosition();
            }

            for (const auto& [key, value]: nbrs)
            {
                for (const auto& el: value)
                {
                    if ((key - 1) == el.first)
                    {
                        max_clique_size = std::max(max_clique_size, key);
                    }
                }
            }

        }
        return max_clique_size;
    }

    static std::vector<std::vector<size_t>> maxCliqueFinding(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
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

    static std::vector<std::set<size_t>> maxCliqueFinding2(my::BitAdjacencyMatrix<NeighboursList>& adjMatr)
    {
        std::vector<std::set<size_t>> allMaxCliques{};
        size_t dimSize = adjMatr.getMatrDimSize();

        for (size_t ind = 0; ind < dimSize; ++ind)
        {
            adjMatr[ind].unset(ind);
        }

        for(size_t ind = 0; ind < dimSize; ++ind)
        {
            auto pathBitset = adjMatr.getLine(ind);
            auto curBitset = pathBitset;
            std::set<size_t> cliqueSet{ ind };

            size_t adjVert = curBitset.getFirstNonZeroPosition();
            std::set<std::pair<size_t, size_t> > vertPowersByPower;
            std::unordered_map<size_t, std::pair<size_t, NeighboursList>> vertPowersByVert;
            std::cout << ind << '\n';
            while (adjVert < dimSize)
            {
                auto bs = adjMatr.getLine(adjVert) & adjMatr.getLine(ind);
                size_t numBr{ countSetBits(bs) };
                vertPowersByVert[adjVert] = std::make_pair(numBr, std::move(bs));
                vertPowersByPower.insert(std::make_pair(numBr, adjVert));
                curBitset.unset(adjVert);
                adjVert = curBitset.getFirstNonZeroPosition();
            }
            auto mainBitset = adjMatr.getLine(ind);
            bool flag = true;
            std::cout << "Into while\n";
            while (flag)
            {
                size_t maxInPower{};
                size_t maxOutPower{};
                size_t minInPower{ dimSize };
                if (vertPowersByPower.empty()) break;
                auto iter = vertPowersByPower.end();
                --iter;
                std::set<size_t> maxVertNbr = vertPowersByVert[iter->second].second.getNeighbours();
                for (const auto& nbr: maxVertNbr)
                {
                    if (vertPowersByVert.find(nbr) != vertPowersByVert.end())
                    {
                        maxInPower = std::max(vertPowersByVert[nbr].first, maxInPower);
                        minInPower = std::max(vertPowersByVert[nbr].first, minInPower);
                    }
                }
                auto maxVertNotNbrBitset = vertPowersByVert[iter->second].second ^ mainBitset;
                maxVertNotNbrBitset.unset(iter->second);
                std::set<size_t> maxVertNotNbr = maxVertNotNbrBitset.getNeighbours();
                for (const auto& nbr: maxVertNotNbr)
                {
                    maxOutPower = vertPowersByVert.find(nbr) != vertPowersByVert.end() ?
                        std::max(vertPowersByVert[nbr].first, maxOutPower) :
                        maxOutPower;
                }


                if (maxInPower <= maxOutPower)
                {
                    vertPowersByVert.erase(iter->second);
                    vertPowersByPower.erase(iter);
                }
                else
                {
                    for(const auto& notNbr: maxVertNotNbr)
                    {
                        vertPowersByPower.erase(std::make_pair(vertPowersByVert[notNbr].first, notNbr));
                        vertPowersByVert.erase(notNbr);
                    }
                    const auto& maxVertBitset = vertPowersByVert[iter->second].second;
                    for(const auto& nbr: maxVertNbr)
                    {
                        vertPowersByVert[nbr].second &= maxVertBitset;
                        vertPowersByVert[nbr].first = countSetBits(vertPowersByVert[nbr].second);
                    }
                }
            }
        }
        return allMaxCliques;
    }
};