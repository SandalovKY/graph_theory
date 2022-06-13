#include "heuristic_max_clique.hpp"


SegundoAlgorithm::bitset_type maxCliqueFindingHeuristic(std::map<size_t, SegundoAlgorithm::bitset_type> adjMatr)
{
    size_t dimSize = adjMatr.begin()->second.getDimSize();

    for (size_t ind = 0; ind < dimSize; ++ind)
    {
        adjMatr[ind].unset(ind);
    }
    SegundoAlgorithm::bitset_type maxClique(dimSize);
    size_t maxCliqueSize{ 0 };

    for(size_t ind = 0; ind < dimSize; ++ind)
    {
        size_t currCliqueSize{ 0 };
        auto pathBitset = adjMatr[ind];
        if (countSetBits(pathBitset) + 1 > maxCliqueSize)
        {
            auto curBitset = pathBitset;
            SegundoAlgorithm::bitset_type cliqueBitset(dimSize);
            cliqueBitset.set(ind);
            ++currCliqueSize;
            size_t adjVert = curBitset.getFirstNonZeroPosition();
            while (adjVert < dimSize && currCliqueSize + countSetBits(pathBitset) > maxCliqueSize)
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
                ++currCliqueSize;
                pathBitset &= adjMatr[nextVert];
                adjVert = pathBitset.getFirstNonZeroPosition();
                curBitset = pathBitset;
            }
            if (currCliqueSize > maxCliqueSize)
            {
                maxClique = cliqueBitset;
                maxCliqueSize = currCliqueSize;
            }
        }
    }
    return maxClique;
}

SegundoAlgorithm::bitset_type maxCliqueFindingHeuristicSimple(std::map<size_t, myBitset<>> adjMatr)
{
    if (adjMatr.empty()) return SegundoAlgorithm::bitset_type(0);

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
        SegundoAlgorithm::bitset_type currMaxClique(numBits);
        currMaxClique.set(ind);
        SegundoAlgorithm::bitset_type nbhd(adjMatr[ind]);
        size_t nextVert = nbhd.getFirstNonZeroPosition();
        while (nextVert < numBits && maxCliqueSize + countSetBits(nbhd) > globalMaxCliqueSize)
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
