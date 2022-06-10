#include "heuristic_max_clique.hpp"


SegundoAlgorithm::bitset_type maxCliqueFindingHeuristic(std::map<size_t, SegundoAlgorithm::bitset_type> adjMatr)
{
    std::vector<SegundoAlgorithm::bitset_type> allMaxCliques{};
    size_t dimSize = adjMatr.begin()->second.getDimSize();

    for (size_t ind = 0; ind < dimSize; ++ind)
    {
        adjMatr[ind].unset(ind);
    }

    size_t maxCliqueSize{ 0 };

    for(size_t ind = 0; ind < dimSize; ++ind)
    {
        bool isFinished{ false };
        size_t iter { 0 };
        while (!isFinished)
        {
            auto pathBitset = adjMatr[ind];
            auto curBitset = pathBitset;
            SegundoAlgorithm::bitset_type cliqueBitset(dimSize, -1);
            cliqueBitset.set(ind);
            size_t adjVert = curBitset.getFirstNonZeroPosition();
            while (adjVert < dimSize)
            {
                size_t nextVert{ adjVert };
                curBitset.unset(adjVert);
                size_t numNbr{ countSetBits(adjMatr[adjVert] & pathBitset) };
                size_t otherVert{ curBitset.getFirstNonZeroPosition() };
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
                cliqueBitset.set(nextVert);
                pathBitset &= adjMatr[nextVert];
                adjVert = pathBitset.getFirstNonZeroPosition();
                curBitset = pathBitset;
            }
            ++iter;
            size_t currCliqueSize{ countSetBits(cliqueBitset) };
            if (currCliqueSize > maxCliqueSize)
            {
                allMaxCliques.push_back(cliqueBitset);
                maxCliqueSize = currCliqueSize;
            }
            if (iter == 1) isFinished = true;
        }
    }

    SegundoAlgorithm::bitset_type maxClique{};
    size_t currMaxCliqueSize{ 0 };

    for (const auto& clique: allMaxCliques)
    {
        size_t currCliqueSize{ countSetBits(clique) };
        if (currCliqueSize > currMaxCliqueSize)
        {
            currMaxCliqueSize = currCliqueSize;
            maxClique = clique;
        }
    }
    return maxClique;
}

SegundoAlgorithm::bitset_type maxCliqueFindingHeuristicSimple(std::map<size_t, myDynamicBitset<>> adjMatr)
{
    if (adjMatr.empty()) return SegundoAlgorithm::bitset_type(0, -1);

    size_t numBits = adjMatr.begin()->second.getDimSize();

    for (size_t ind = 0; ind < numBits; ++ind)
    {
        adjMatr[ind].unset(ind);
    }

    size_t globalMaxCliqueSize{ 0 };
    SegundoAlgorithm::bitset_type globalMaxClique{};

    for(size_t ind = 0; ind < numBits; ++ind)
    {
        size_t maxCliqueSize{ 1 };
        SegundoAlgorithm::bitset_type currMaxClique(numBits, -1);
        currMaxClique.set(ind);
        SegundoAlgorithm::bitset_type nbhd(adjMatr[ind]);
        size_t nextVert = nbhd.getFirstNonZeroPosition();
        while (nextVert < numBits)
        {
            ++maxCliqueSize;
            currMaxClique.set(nextVert);
            nbhd &= adjMatr[nextVert];
            nextVert = nbhd.getFirstNonZeroPosition();
        }
        if (maxCliqueSize > globalMaxCliqueSize)
        {
            globalMaxClique = currMaxClique;
            globalMaxCliqueSize = maxCliqueSize;
        }
    }
    return globalMaxClique;
}

boost::dynamic_bitset<> maxCliqueFindingHeuristicSimpleBoost(std::map<size_t, boost::dynamic_bitset<>> adjMatr)
{
    if (adjMatr.empty()) return boost::dynamic_bitset();

    size_t numBits = adjMatr.begin()->second.size();

    for (size_t ind = 0; ind < numBits; ++ind)
    {
        adjMatr[ind].set(ind, false);
    }

    size_t globalMaxCliqueSize{ 0 };
    boost::dynamic_bitset<> globalMaxClique{};

    for(size_t ind = 0; ind < numBits; ++ind)
    {
        size_t maxCliqueSize{ 1 };
        boost::dynamic_bitset<> currMaxClique(numBits);
        currMaxClique.set(ind, true);
        boost::dynamic_bitset<> nbhd(adjMatr[ind]);
        size_t nextVert = nbhd.find_first();
        while (nextVert < numBits)
        {
            ++maxCliqueSize;
            currMaxClique.set(nextVert);
            nbhd &= adjMatr[nextVert];
            nextVert = nbhd.find_first();
        }
        if (maxCliqueSize > globalMaxCliqueSize)
        {
            globalMaxClique = currMaxClique;
            globalMaxCliqueSize = maxCliqueSize;
        }
    }
    return globalMaxClique;
}
