#include "segundo_max_clique.hpp"

#include <iostream>

int32_t step_count{ 0 };

void SegundoAlgorithm::runMaxCliqueFinding(std::map<size_t, bitset_type> adjMatr, bool useModAlgorithm)
{
    step_count = 0;
    this->maxClique.clear();
    std::set<size_t> defSet{};
    if (useModAlgorithm)
    {
        std::map<size_t, bitset_type> copyMatr = this->coloringUsingAdditionalMatrix(adjMatr, 0);
        this->maxCliqueFindingSegundoUsingAdditionalMatrix(adjMatr, copyMatr, defSet);
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
    std::cout << "Num steps: " << step_count << std::endl;
}

void SegundoAlgorithm::runTestAlgorithm()
{
    step_count = 0;
    size_t numBits{ 0 };
    if (!this->globalAdjMatr.empty())
    {
        numBits = this->globalAdjMatr.begin()->second.getDimSize();
    }
    bitset_type inputVerts(numBits, -1);
    bitset_type currMaxClique(numBits, -1);
    inputVerts.all2one();
    index_lines copyMatr = this->coloringUsingAdditionalMatrix(this->globalAdjMatr, 0);
    this->maxCliqueSegTest(inputVerts, copyMatr, currMaxClique);
    std::cout << "Num steps: " << step_count << std::endl;
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

SegundoAlgorithm::index_lines SegundoAlgorithm::coloringUsingAdditionalMatrix(index_lines& adjMatr, int32_t minCol)
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

SegundoAlgorithm::SegundoAlgorithm(const index_lines& adjMatr)
    : globalAdjMatr(adjMatr)
{
    if (!globalAdjMatr.empty())
    {
        this->globalMaxClique = bitset_type(globalAdjMatr.begin()->second.getDimSize(), -1);
    }
}

void SegundoAlgorithm::maxCliqueSegTest(bitset_type searchSubgraph, index_lines& allowedVerts, bitset_type& currMaxClique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = *(--allowedVerts.end());

        size_t lineInd = currLine.first;

        allowedVerts.erase(lineInd);

        int32_t currCliqueSize = countSetBits(currMaxClique);
        int32_t maxCliqueSize = countSetBits(this->globalMaxClique);

        if (currCliqueSize + currLine.second.getColor() > maxCliqueSize)
        {
            searchSubgraph.unset(lineInd);
            currMaxClique.set(lineInd);
            ++currCliqueSize;

            bitset_type& line = currLine.second;

            line &= searchSubgraph;

            index_lines nearVerts = this->getNeighboursTest(line);
            if (!nearVerts.empty())
            {
                index_lines coloredVerts = this->coloringUsingAdditionalMatrix(nearVerts, maxCliqueSize - currCliqueSize + 1);
                this->maxCliqueSegTest(line, coloredVerts, currMaxClique);
            }
            else
            {
                if (currCliqueSize > maxCliqueSize)
                {
                    this->globalMaxClique = currMaxClique;
                }
            }
            currMaxClique.unset(lineInd);
            --currCliqueSize;
        }
    }
}

void SegundoAlgorithm::maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr,
    std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = *(--allowedVerts.end());
        allowedVerts.erase(currLine.first);
        // adjMatr.erase(currLine.first);
        
        if (currMaxCLique.size() + currLine.second.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLine.first);
            // Get all near verticies that was conjuncted with currLine
            adjMatr.erase(currLine.first);
            index_lines nearVerts = this->getNeighbours(adjMatr, currLine.second);
            if (!nearVerts.empty())
            {
                index_lines coloredVerts = this->coloring(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
                maxCliqueFindingSegundo(nearVerts, coloredVerts, currMaxCLique);
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

void SegundoAlgorithm::maxCliqueFindingSegundoUsingAdditionalMatrix(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = *(--allowedVerts.end());
        allowedVerts.erase(currLine.first);
        // adjMatr.erase(currLine.first);
        
        if (currMaxCLique.size() + currLine.second.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLine.first);
            // Get all near verticies that was conjuncted with currLine
            adjMatr.erase(currLine.first);
            index_lines nearVerts = this->getNeighboursUsingAdditionalMatrix(adjMatr, currLine.second);

            if (!nearVerts.empty())
            {
                index_lines coloredVerts = this->coloringUsingAdditionalMatrix(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
                maxCliqueFindingSegundoUsingAdditionalMatrix(nearVerts, coloredVerts, currMaxCLique);
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

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighboursUsingAdditionalMatrix(index_lines& adjMatr, bitset_type& bitset)
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
            retMap.insert({ firstSetBitPos, std::move(line) });
        }
        localBitset.unset(firstSetBitPos);
        firstSetBitPos = localBitset.getFirstNonZeroPosition();
    }
    return retMap;
}

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighboursTest(bitset_type& currBitset)
{
    index_lines retVerticies{};
    size_t dimSize = currBitset.getDimSize();
    bitset_type localBitset = currBitset;
    size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
    while (firstSetBitPos < dimSize)
    {
        if (this->globalAdjMatr.contains(firstSetBitPos))
        {
            bitset_type line = this->globalAdjMatr[firstSetBitPos];
            line &= currBitset;
            retVerticies.insert({ firstSetBitPos, std::move(line) });
        }
        localBitset.unset(firstSetBitPos);
        firstSetBitPos = localBitset.getFirstNonZeroPosition();
    }
    return retVerticies;
}