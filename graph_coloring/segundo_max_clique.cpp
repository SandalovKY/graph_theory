#include <set>
#include <map>
#include "segundo_max_clique.hpp"

void SegundoAlgorithm::runMaxCliqueFinding(std::map<size_t, bitset_type>& adjMatr)
{
    for (auto& vertex: adjMatr)
    {
        auto coloredVertexes = this->coloring(this->getNeighbours(adjMatr, vertex.second), 0);
        if (!coloredVertexes.empty())
        {
            vertex.second.setColor((--coloredVertexes.end())->second.getColor());
        }
    }
    std::set<size_t> defSet{};
    this->maxCliqueFindingSegundo(adjMatr, defSet);
}

std::map<size_t, SegundoAlgorithm::bitset_type> SegundoAlgorithm::coloring(std::map<size_t, std::pair<bitset_type, bool>> adjMatr, size_t currMaxCliqueSize)
{
    std::map<size_t, bitset_type> resCols{};
    size_t dimSize = adjMatr.size();
    size_t currColor{ 0 };

    for (auto& record: adjMatr)
    {
        auto& recordVal = record.second;
        if (!recordVal.second)
        {
            recordVal.first.setColor(currColor);
            SegundoAlgorithm::bitset_type bitSet = recordVal.first;
            if (this->maxClique.size() - (currMaxCliqueSize - 1) <= currColor)
            {
                resCols.insert({record.first, ~record.second.first});
            }
            recordVal.second = true;
            size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
            while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
            {
                if (adjMatr.contains(firstSetBitPos) && !adjMatr[firstSetBitPos].second)
                {
                    bitSet &= adjMatr[firstSetBitPos].first;
                    adjMatr[firstSetBitPos].first.setColor(currColor);
                    if (this->maxClique.size() - (currMaxCliqueSize - 1) <= currColor)
                    {
                        resCols.insert({ firstSetBitPos, ~adjMatr[firstSetBitPos].first });
                    }
                    adjMatr[firstSetBitPos].second = true;
                }
                bitSet.unset(firstSetBitPos);
                firstSetBitPos = bitSet.getFirstNonZeroPosition();
            }
            currColor += 1;
        }
    }
    return resCols;
}

void SegundoAlgorithm::maxCliqueFindingSegundo(std::map<size_t, SegundoAlgorithm::bitset_type>& adjMatr, std::set<size_t>& currMaxCLique)
{
    while (!adjMatr.empty())
    {
        auto currLine = *(--adjMatr.end());
        std::cout << currLine.second.getId() << std::endl;
        adjMatr.erase(currLine.first);
        if (currMaxCLique.size() + currLine.second.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLine.first);
            std::map<size_t, SegundoAlgorithm::bitset_type> coloredVerts = this->coloring(this->getNeighbours(adjMatr, currLine.second), currMaxCLique.size());
            if (!coloredVerts.empty())
            {
                SegundoAlgorithm::maxCliqueFindingSegundo(coloredVerts, currMaxCLique);
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
        else return;
    }
}

std::map<size_t, std::pair<SegundoAlgorithm::bitset_type, bool>> SegundoAlgorithm::getNeighbours(std::map<size_t, SegundoAlgorithm::bitset_type>& adjMatr, SegundoAlgorithm::bitset_type& bitset)
{
    std::map<size_t, std::pair<SegundoAlgorithm::bitset_type, bool>> retMap{};
    size_t dimSize = bitset.getDimSize();
    bitset.unset(bitset.getId());
    SegundoAlgorithm::bitset_type localBitset = bitset;
    size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
    while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
    {
        if (adjMatr.contains(firstSetBitPos))
        {
            SegundoAlgorithm::bitset_type line = adjMatr[firstSetBitPos];
            line &= bitset;
            // line.setId(firstSetBitPos);
            line.set(firstSetBitPos);
            ~line;
            retMap.insert({ firstSetBitPos, { std::move(line), false } });
        }
        localBitset.unset(firstSetBitPos);
        firstSetBitPos = localBitset.getFirstNonZeroPosition();
    }
    // bitset.set(bitset.getId());
    return retMap;
}