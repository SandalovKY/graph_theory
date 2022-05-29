#include "heuristic_max_clique.hpp"


std::set<size_t> maxCliqueFindingHeuristic(std::map<size_t, myDynamicBitset<>> adjMatr)
{
    std::vector<std::vector<size_t>> allMaxCliques{};
    size_t dimSize = adjMatr.begin()->second.getDimSize();

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

    return std::set<size_t>(maxCliqueVector.begin(), maxCliqueVector.end());
}