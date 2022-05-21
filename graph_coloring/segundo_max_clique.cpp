#include <set>
#include <map>
#include "segundo_max_clique.hpp"

void SegundoAlgorithm::runMaxCliqueFinding(std::map<size_t, bitset_type>& adjMatr)
{
    std::set<size_t> defSet{};
    std::map<size_t, bitset_type> copyMatr = adjMatr;
    this->maxCliqueFindingSegundo(adjMatr, copyMatr, defSet);
}

SegundoAlgorithm::index_lines SegundoAlgorithm::coloring(index_lines& adjMatr, int32_t currMaxCliqueSize)
{
    std::map<size_t, bool> isIncludedArray{};
    std::map<size_t, bitset_type> resCols{};
    std::vector<bitset_type> allowedVerts{};
    size_t dimSize{ 0 };
    if (!adjMatr.empty())
    {
        dimSize = adjMatr.begin()->second.getDimSize();
    }
    int32_t currColor{ 1 };

    for (auto& record: adjMatr)
    {
        auto& recordVal = record.second;
        if (!isIncludedArray[record.first])
        {
            recordVal.setColor(currColor);
            SegundoAlgorithm::bitset_type bitSet = recordVal;
            ~record.second;
            if ((int32_t)(this->maxClique.size() - currMaxCliqueSize + 1) <= currColor)
            {
                resCols.insert({record.first, record.second});
            }
            isIncludedArray[record.first] = true;
            size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
            while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
            {
                if (adjMatr.contains(firstSetBitPos) && !isIncludedArray[firstSetBitPos])
                {
                    bitSet &= adjMatr[firstSetBitPos];
                    adjMatr[firstSetBitPos].setColor(currColor);
                    ~adjMatr[firstSetBitPos];
                    if ((int32_t)(this->maxClique.size() - currMaxCliqueSize + 1) <= currColor)
                    {
                        resCols.insert({ firstSetBitPos, adjMatr[firstSetBitPos] });
                    }
                    isIncludedArray[firstSetBitPos] = true;
                }
                bitSet.unset(firstSetBitPos);
                firstSetBitPos = bitSet.getFirstNonZeroPosition();
            }
            currColor += 1;
        }
    }
    return resCols;
}

void SegundoAlgorithm::maxCliqueFindingSegundo(std::map<size_t, SegundoAlgorithm::bitset_type>& adjMatr,
    std::map<size_t, SegundoAlgorithm::bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique)
{
    while (!allowedVerts.empty())
    {
        auto currLine = *(--allowedVerts.end());
        allowedVerts.erase(currLine.first);
        currMaxCLique.insert(currLine.first);

        index_lines nearVerts = this->getNeighbours(adjMatr, currLine.second);
        
        std::map<size_t, SegundoAlgorithm::bitset_type> coloredVerts = this->coloring(nearVerts, currMaxCLique.size());
        if (!coloredVerts.empty())
        {
            if (currMaxCLique.size() + ((--coloredVerts.end())->second.getColor()) > this->maxClique.size())
                SegundoAlgorithm::maxCliqueFindingSegundo(nearVerts, coloredVerts, currMaxCLique);
        }
        else
        {
            if (currMaxCLique.size() > this->maxClique.size())
            {
                this->maxClique = currMaxCLique;
            }
        }
        currMaxCLique.erase(currLine.first);
    }
}

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighbours(const index_lines& adjMatr, bitset_type& bitset)
{
    index_lines retMap{};
    size_t dimSize = bitset.getDimSize();
    bitset.unset(bitset.getId());
    bitset_type localBitset = bitset;
    size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
    while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
    {
        if (adjMatr.contains(firstSetBitPos))
        {
            bitset_type line = adjMatr.at(firstSetBitPos);
            line &= bitset;
            line.set(firstSetBitPos);
            ~line;
            retMap.insert({ firstSetBitPos, std::move(line) });
        }
        localBitset.unset(firstSetBitPos);
        firstSetBitPos = localBitset.getFirstNonZeroPosition();
    }
    return retMap;
}