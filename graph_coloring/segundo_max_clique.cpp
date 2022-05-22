#include <set>
#include <map>
#include "segundo_max_clique.hpp"

void SegundoAlgorithm::runMaxCliqueFinding(std::map<size_t, bitset_type>& adjMatr, bool useModAlgorithm)
{
    this->maxClique.clear();
    std::set<size_t> defSet{};
    if (useModAlgorithm)
    {
        std::map<size_t, bitset_type> copyMatr = this->coloring_mod_1(adjMatr, 0);
        this->maxCliqueFindingSegundo_mod_1(adjMatr, copyMatr, defSet);
    }
    else
    {
        std::map<size_t, bitset_type> copyMatr = adjMatr;
        for (auto& line: copyMatr)
        {
            line.second.set(line.second.getId());
            ~line.second;
        }
        std::map<size_t, bitset_type> coloredMatr = this->coloring(copyMatr, 0);
        this->maxCliqueFindingSegundo(adjMatr, coloredMatr, defSet);
    }
}

SegundoAlgorithm::index_lines SegundoAlgorithm::coloring(index_lines& adjMatr, int32_t minCol)
{
    std::map<size_t, bool> isIncludedArray{};
    index_lines resCols{};
    size_t dimSize{ 0 };
    if (!adjMatr.empty())
    {
        dimSize = adjMatr.begin()->second.getDimSize();
    }
    int32_t currColor{ 1 };

    for (auto& record: adjMatr)
    {
        auto& recordVal = record.second;
        size_t recordInd = record.first;
        if (!isIncludedArray[recordInd])
        {
            recordVal.setColor(currColor);
            SegundoAlgorithm::bitset_type bitSet = recordVal;
            ~recordVal;
            if (currColor >= minCol)
            {
                resCols.insert({recordInd, recordVal});
            }
            isIncludedArray[recordInd] = true;
            size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
            while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
            {
                if (adjMatr.contains(firstSetBitPos) && !isIncludedArray[firstSetBitPos])
                {
                    bitSet &= adjMatr[firstSetBitPos];
                    adjMatr[firstSetBitPos].setColor(currColor);
                    ~adjMatr[firstSetBitPos];
                    if (currColor >= minCol)
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

SegundoAlgorithm::index_lines SegundoAlgorithm::coloring_mod_1(index_lines& adjMatr, int32_t minCol)
{
    index_lines resCols{};

    size_t numBits{ 0 };
    if (!adjMatr.empty())
    {
        numBits = adjMatr.begin()->second.getDimSize();
    }
    std::vector<bitset_type> tabuCols(adjMatr.size(), bitset_type(numBits, -1));

    for (auto& record: adjMatr)
    {
        auto& recordVal = record.second;
        size_t recordInd = record.first;
        bitset_type tmpBitset(numBits, recordInd);
        tmpBitset.set(recordInd);
        int32_t color{ 0 };
        bitset_type cmpBitset = tabuCols[color] & tmpBitset; 
        while (countSetBits(cmpBitset))
        {
            ++color;
            cmpBitset = tabuCols[color] & tmpBitset;
        }
        recordVal.setColor(color + 1);
        tabuCols[color] |= recordVal;
        if (color + 1 >= minCol)
        {
            resCols.insert({ recordInd, recordVal });
        }
    }

    return resCols;
}

void SegundoAlgorithm::maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr,
    std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique)
{
    while (!allowedVerts.empty())
    {
        auto currLine = *(--allowedVerts.end());
        allowedVerts.erase(currLine.first);
        
        if (currMaxCLique.size() + currLine.second.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLine.first);
            // Get all near verticies that was conjuncted with currLine
            index_lines nearVerts = this->getNeighbours(adjMatr, currLine.second);
            
            std::map<size_t, SegundoAlgorithm::bitset_type> coloredVerts = this->coloring(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
            if (!coloredVerts.empty())
            {
                // if (currMaxCLique.size() + ((--coloredVerts.end())->second.getColor()) > this->maxClique.size())
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
}

void SegundoAlgorithm::maxCliqueFindingSegundo_mod_1(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique)
{
    while (!allowedVerts.empty())
    {
        auto currLine = *(--allowedVerts.end());
        allowedVerts.erase(currLine.first);
        
        if (currMaxCLique.size() + currLine.second.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLine.first);
            // Get all near verticies that was conjuncted with currLine
            index_lines nearVerts = this->getNeighbours_mod_1(adjMatr, currLine.second);
            
            std::map<size_t, SegundoAlgorithm::bitset_type> coloredVerts = this->coloring_mod_1(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
            if (!coloredVerts.empty())
            {
                // if (currMaxCLique.size() + ((--coloredVerts.end())->second.getColor()) > this->maxClique.size())
                SegundoAlgorithm::maxCliqueFindingSegundo_mod_1(nearVerts, coloredVerts, currMaxCLique);
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
}

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighbours(index_lines& adjMatr, bitset_type& bitset)
{
    index_lines retMap{};
    size_t dimSize = bitset.getDimSize();
    bitset.unset(bitset.getId());
    bitset_type localBitset = bitset;
    size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
    while (firstSetBitPos < dimSize)
    {
        if (adjMatr.contains(firstSetBitPos))
        {
            bitset_type line = adjMatr[firstSetBitPos];
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

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighbours_mod_1(index_lines& adjMatr, bitset_type& bitset)
{
    index_lines retMap{};
    size_t dimSize = bitset.getDimSize();
    bitset.unset(bitset.getId());
    bitset_type localBitset = bitset;
    size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
    while (firstSetBitPos < dimSize)
    {
        if (adjMatr.contains(firstSetBitPos))
        {
            bitset_type line = adjMatr[firstSetBitPos];
            line &= bitset;
            line.set(firstSetBitPos);
            retMap.insert({ firstSetBitPos, std::move(line) });
        }
        localBitset.unset(firstSetBitPos);
        firstSetBitPos = localBitset.getFirstNonZeroPosition();
    }
    return retMap;
}