#include "segundo_max_clique.hpp"
#include "heuristic_max_clique.hpp"

#include <iostream>

int32_t step_count{ 0 };

SegundoAlgorithm::SegundoAlgorithm(index_lines adjMatr)
    : globalAdjMatr(std::move(adjMatr))
{
    if (!globalAdjMatr.empty())
    {
        this->globalMaxClique = bitset_type(globalAdjMatr.begin()->second.getDimSize(), -1);
    }
}

void SegundoAlgorithm::runTestAlgorithm(Algorithms alg)
{
    step_count = 0;
    size_t numBits{ this->globalMaxClique.getDimSize() };
    this->maxClique = { 0, 1 };
    std::set<size_t> defSet{};
    index_lines copyMatr = this->globalAdjMatr;
    index_lines allowedMatr = this->globalAdjMatr;
    index_lines_with_order coloredMatr{};
    // index_lines_with_order coloredMap{};

    bitset_type inputVerts(numBits, -1);
    bitset_type currMaxClique(numBits, -1);
    inputVerts.all2one();

    switch (alg)
    {
    case Algorithms::BoostedReferenceAlgorithm:
        this->maxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
    case Algorithms::Reference:
        coloredMatr = this->coloring(allowedMatr, 3);
        this->maxCliqueFindingSegundoReference(inputVerts, coloredMatr, defSet);
        break;
    case Algorithms::BoostedModifiedAlgorithm:
        this->maxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
    case Algorithms::Modified:
        coloredMatr = this->coloringUsingAdditionalMatrix(allowedMatr, 3);
        this->maxCliqueFindingSegundoUsingAdditionalMatrix(copyMatr, coloredMatr, defSet);
        break;
    case Algorithms::ModifiedWithInputBitset:
        coloredMatr = this->coloringUsingAdditionalMatrix(allowedMatr, 3);
        this->maxCliqueSegTest(inputVerts, coloredMatr, currMaxClique);
        break;
    default:
        break;
    }
    std::cout << "Num steps: " << step_count << std::endl;
}

SegundoAlgorithm::index_lines_with_order SegundoAlgorithm::coloring(index_lines& adjMatr, int32_t minCol)
{
    std::map<size_t, bool> isIncludedArray{};
    index_lines_with_order resColsOrdered{};
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
            recordVal.set(recordInd);
            ~recordVal;
            bitset_type bitSet = recordVal;
            ~recordVal;
            if (currColor >= minCol)
            {
                resColsOrdered.push_back(recordVal);
            }
            isIncludedArray[recordInd] = true;
            size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
            while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
            {
                if (adjMatr.contains(firstSetBitPos) && !isIncludedArray[firstSetBitPos])
                {
                    adjMatr[firstSetBitPos].set(firstSetBitPos);
                    ~adjMatr[firstSetBitPos];
                    bitSet &= adjMatr[firstSetBitPos];
                    adjMatr[firstSetBitPos].setColor(currColor);
                    ~adjMatr[firstSetBitPos];
                    if (currColor >= minCol)
                    {
                        resColsOrdered.push_back(adjMatr[firstSetBitPos]);
                    }
                    isIncludedArray[firstSetBitPos] = true;
                }
                bitSet.unset(firstSetBitPos);
                firstSetBitPos = bitSet.getFirstNonZeroPosition();
            }
            currColor += 1;
        }
    }
    return resColsOrdered;
}

SegundoAlgorithm::index_lines_with_order SegundoAlgorithm::coloringUsingAdditionalMatrix(index_lines& adjMatr, int32_t minCol)
{
    index_lines_with_order resCols{};
    // index_lines_with_order_mm resColsMap{};

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
            // resColsMap.insert({ color + 1, recordVal });
            resCols.push_back(recordVal);
        }
    }
    return resCols;
}

void SegundoAlgorithm::maxCliqueSegTest(bitset_type searchSubgraph, index_lines_with_order& allowedVerts, bitset_type& currMaxClique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = allowedVerts.back();
        // auto currLine = *(--allowedVerts.end());
        size_t lineInd = currLine.getId();

        // allowedVerts.erase((--allowedVerts.end()));
        // searchSubgraph.unset(lineInd);
        allowedVerts.pop_back();

        int32_t currCliqueSize = countSetBits(currMaxClique);
        int32_t maxCliqueSize = countSetBits(this->globalMaxClique);

        if (currCliqueSize + currLine.getColor() > maxCliqueSize)
        {
            searchSubgraph.unset(lineInd);
            currMaxClique.set(lineInd);
            ++currCliqueSize;

            bitset_type& line = currLine;

            line &= searchSubgraph;

            index_lines nearVerts = this->getNeighboursBitsetInput(line);
            if (!nearVerts.empty())
            {
                index_lines_with_order coloredVerts = this->coloringUsingAdditionalMatrix(nearVerts, maxCliqueSize - currCliqueSize + 1);
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
        // else break;
    }
}

void SegundoAlgorithm::maxCliqueFindingSegundoReference(bitset_type searchSubgraph,
    index_lines_with_order& allowedVerts, std::set<size_t>& currMaxCLique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = allowedVerts.back();
        size_t currLineInd = currLine.getId();

        allowedVerts.pop_back();
        searchSubgraph.unset(currLineInd);
        
        if (currMaxCLique.size() + currLine.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLineInd);

            bitset_type& line = currLine;
            line &= searchSubgraph;

            // Get all near verticies that was conjuncted with currLine
            index_lines nearVerts = this->getNeighboursBitsetInput(line);
            if (!nearVerts.empty())
            {
                index_lines_with_order coloredVerts = this->coloring(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
                maxCliqueFindingSegundoReference(line, coloredVerts, currMaxCLique);
            }
            else
            {
                if (currMaxCLique.size() > this->maxClique.size())
                {
                    this->maxClique = currMaxCLique;
                }
            }
            currMaxCLique.erase(currLineInd);
        }
        else break;
    }
}

void SegundoAlgorithm::maxCliqueFindingSegundoUsingAdditionalMatrix(std::map<size_t, bitset_type>& adjMatr,
        index_lines_with_order& allowedVerts, std::set<size_t>& currMaxCLique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = allowedVerts.back();
        // auto currLine = *(--allowedVerts.end());
        size_t currLineInd = currLine.getId();
        
        // allowedVerts.erase((--allowedVerts.end()));
        // adjMatr.erase(currLineInd);
        allowedVerts.pop_back();
        
        if (currMaxCLique.size() + currLine.getColor() > this->maxClique.size())
        {
            currMaxCLique.insert(currLineInd);
            // Get all near verticies that was conjuncted with currLine
            adjMatr.erase(currLineInd);
            index_lines nearVerts = this->getNeighbours(adjMatr, currLine);

            if (!nearVerts.empty())
            {
                index_lines_with_order coloredVerts = this->coloringUsingAdditionalMatrix(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
                maxCliqueFindingSegundoUsingAdditionalMatrix(nearVerts, coloredVerts, currMaxCLique);
            }
            else
            {
                if (currMaxCLique.size() > this->maxClique.size())
                {
                    this->maxClique = currMaxCLique;
                }
            }
            currMaxCLique.erase(currLineInd);
        }
        // else break;
    }
}

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighbours(index_lines& adjMatr, bitset_type& currBitset)
{
    index_lines retVerticies{};
    size_t dimSize = currBitset.getDimSize();
    currBitset.unset(currBitset.getId());
    bitset_type localBitset = currBitset;
    size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
    while (firstSetBitPos < dimSize)
    {
        if (adjMatr.contains(firstSetBitPos))
        {
            bitset_type line = adjMatr[firstSetBitPos];
            line &= currBitset;
            retVerticies.insert({ firstSetBitPos, std::move(line) });
        }
        localBitset.unset(firstSetBitPos);
        firstSetBitPos = localBitset.getFirstNonZeroPosition();
    }
    return retVerticies;
}

SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighboursBitsetInput(bitset_type& currBitset)
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
