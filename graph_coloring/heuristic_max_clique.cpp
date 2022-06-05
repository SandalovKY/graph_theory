#include "heuristic_max_clique.hpp"


SegundoAlgorithm::bitset_type maxCliqueFindingHeuristic(std::map<size_t, myDynamicBitset<>> adjMatr)
{
    std::vector<std::vector<size_t>> allMaxCliques{};
    size_t dimSize = adjMatr.begin()->second.getDimSize();
    SegundoAlgorithm::bitset_type retBitset(dimSize, -1);

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
            auto pathBitset = adjMatr[ind];
            auto curBitset = pathBitset;
            std::vector<size_t> cliqueVector{ ind };
            size_t adjVert = pathBitset.getFirstNonZeroPosition();
            size_t iter {0};
            while (adjVert < dimSize)
            {
                size_t nextVert{ adjVert };
                curBitset.unset(adjVert);
                size_t numNbr{ countSetBits(adjMatr[adjVert] & pathBitset) };
                size_t otherVert{ static_cast<size_t>(curBitset.getFirstNonZeroPosition()) };
                while (otherVert < dimSize)
                {
                    size_t numNbrForOtherVert = countSetBits(adjMatr[otherVert] & pathBitset);
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
                pathBitset &= adjMatr[nextVert];
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
                    adjMatr[cliqueVector[ind]].unset(initVert);
                    adjMatr[initVert].unset(cliqueVector[ind]);
                }
            }
            else
            {
                isFinished = true;
                allMaxCliques.emplace_back(std::move(cliqueVector));
            }
        }
    }
    std::set<size_t> maxClique{};

    std::vector<size_t> maxCliqueVector{};
    size_t maxCliqueSize{ 0 };

    for (const auto& clique: allMaxCliques)
    {
        if (clique.size() > maxCliqueSize)
        {
            maxCliqueSize = clique.size();
            maxCliqueVector = clique;
        }
    }
    for (const auto& vert: maxCliqueVector)
    {
        retBitset.set(vert);
    }
    return retBitset;
}

SegundoAlgorithm::bitset_type maxCliqueFindingHeuristicSimple(std::map<size_t, myDynamicBitset<>> adjMatr)
{
    SegundoAlgorithm::bitset_type retBitset(adjMatr.begin()->second.getDimSize(), -1);
    if (adjMatr.empty()) return retBitset;

    size_t numBits = adjMatr.begin()->second.getDimSize();

    for (size_t ind = 0; ind < numBits; ++ind)
    {
        adjMatr[ind].unset(ind);
    }

    size_t globalMaxCliqueSize{ 0 };
    std::set<size_t> globalMaxClique{};

    for(size_t ind = 0; ind < numBits; ++ind)
    {
        size_t maxCliqueSize{ 1 };
        std::set<size_t> currMaxClique{ ind };
        SegundoAlgorithm::bitset_type nbhd(adjMatr[ind]);
        size_t nextVert = nbhd.getFirstNonZeroPosition();
        while (nextVert < numBits)
        {
            ++maxCliqueSize;
            currMaxClique.insert(nextVert);
            nbhd &= adjMatr[nextVert];
            nextVert = nbhd.getFirstNonZeroPosition();
        }
        if (maxCliqueSize > globalMaxCliqueSize)
        {
            globalMaxClique = currMaxClique;
        }
    }

    for (const auto& vert: globalMaxClique)
    {
        retBitset.set(vert);
    }
    return retBitset;
}
