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

void SegundoAlgorithm::runTestAlgorithm(Algorithms algorithm)
{
    if (this->globalAdjMatr.empty()) return;

    this->globalMaxClique.all2zero();
    step_count = 0;
    size_t numBits{ this->globalMaxClique.getDimSize() };
    std::vector<std::pair<size_t, size_t>> coloredVec{};

    bitset_type inputVerts(numBits, -1);
    bitset_type currMaxClique(numBits, -1);
    inputVerts.all2one();

    switch (algorithm)
    {
    case Algorithms::BoostedReferenceAlgorithm:
        this->globalMaxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
    case Algorithms::Reference:
        coloredVec = this->coloring(inputVerts.getNeighbours(), 3, inputVerts);
        this->maxCliqueFindingSegundoReference(inputVerts, coloredVec, currMaxClique);
        break;
    case Algorithms::BoostedModifiedAlgorithm:
        this->globalMaxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
    case Algorithms::Modified:
        coloredVec = this->retColoring(inputVerts.getNeighbours(), 3);
        this->maxCliqueSegTest(inputVerts, coloredVec, currMaxClique);
        break;
    default:
        break;
    }
    std::cout << "Num steps: " << step_count << std::endl;
}

std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::coloring(const std::set<size_t>& adjMatr, int32_t minCol, const bitset_type& currVerts)
{
    std::map<size_t, bool> isIncludedArray{};
    std::vector<std::pair<size_t, size_t>> retColored{};

    if (this->globalAdjMatr.empty())
    {
        return retColored;
    }
    size_t dimSize{ this->globalAdjMatr.begin()->second.getDimSize() };
    int32_t currColor{ 1 };

    for (auto& record: adjMatr)
    {
        auto recordVal = this->globalAdjMatr[record];
        size_t recordInd = record;
        if (!isIncludedArray[recordInd])
        {
            recordVal.setColor(currColor);
            recordVal.set(recordInd);
            ~recordVal;
            bitset_type bitSet = recordVal;
            ~recordVal;
            if (currColor >= minCol)
            {
                retColored.push_back({ recordInd, currColor });
            }
            isIncludedArray[recordInd] = true;
            size_t firstSetBitPos = bitSet.getFirstNonZeroPosition();
            while (firstSetBitPos < dimSize && firstSetBitPos >= 0)
            {
                if (adjMatr.contains(firstSetBitPos) && !isIncludedArray[firstSetBitPos])
                {
                    bitset_type tmpBitset = this->globalAdjMatr[firstSetBitPos];
                    tmpBitset &= currVerts;
                    tmpBitset.set(firstSetBitPos);
                    ~tmpBitset;
                    bitSet &= tmpBitset;
                    if (currColor >= minCol)
                    {
                        retColored.push_back({ firstSetBitPos, currColor });
                    }
                    isIncludedArray[firstSetBitPos] = true;
                }
                bitSet.unset(firstSetBitPos);
                firstSetBitPos = bitSet.getFirstNonZeroPosition();
            }
            currColor += 1;
        }
    }
    return retColored;
}

std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::retColoring(const std::set<size_t>& currVerts, int32_t minCol)
{
    std::vector<std::pair<size_t, size_t>> retVector{};
    if (this->globalAdjMatr.empty())
    {
        return retVector; 
    }
    size_t numBits{ this->globalAdjMatr.begin()->second.getDimSize() };

    std::vector<bitset_type> tabuCols(currVerts.size(), bitset_type(numBits, -1));

    for (auto& vert: currVerts)
    {
        if (vert >= numBits) break;
        auto recordVal = this->globalAdjMatr[vert];
        size_t recordInd = vert;
        bitset_type tmpBitset(numBits, recordInd);
        tmpBitset.set(recordInd);
        int32_t color{ 0 };
        bitset_type cmpBitset = tabuCols[color] & tmpBitset;
        while (countSetBits(cmpBitset))
        {
            ++color;
            cmpBitset = tabuCols[color] & tmpBitset;
        }
        tabuCols[color] |= recordVal;
        if (color + 1 >= minCol)
        {
            retVector.push_back({ recordInd, color + 1 });
        }
    }

    return retVector;
}

// SegundoAlgorithm::index_lines_with_order SegundoAlgorithm::coloringUsingAdditionalMatrix(index_lines& adjMatr, int32_t minCol)
// {
//     index_lines_with_order resCols{};

//     size_t numBits{ 0 };
//     if (!adjMatr.empty())
//     {
//         numBits = adjMatr.begin()->second.getDimSize();
//     }
//     std::vector<bitset_type> tabuCols(adjMatr.size(), bitset_type(numBits, -1));

//     for (auto& record: adjMatr)
//     {
//         auto& recordVal = record.second;
//         size_t recordInd = record.first;
//         bitset_type tmpBitset(numBits, recordInd);
//         tmpBitset.set(recordInd);
//         int32_t color{ 0 };
//         bitset_type cmpBitset = tabuCols[color] & tmpBitset; 
//         while (countSetBits(cmpBitset))
//         {
//             ++color;
//             cmpBitset = tabuCols[color] & tmpBitset;
//         }
//         recordVal.setColor(color + 1);
//         tabuCols[color] |= recordVal;
//         if (color + 1 >= minCol)
//         {
//             resCols.push_back(recordVal);
//         }
//     }
//     return resCols;
// }

void SegundoAlgorithm::maxCliqueSegTest(bitset_type searchSubgraph, std::vector<std::pair<size_t, size_t>>& allowedVerts, bitset_type& currMaxClique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = allowedVerts.back();
        size_t currlineInd = currLine.first;

        allowedVerts.pop_back();

        int32_t currCliqueSize = countSetBits(currMaxClique);
        int32_t maxCliqueSize = countSetBits(this->globalMaxClique);

        if (currCliqueSize + currLine.second > maxCliqueSize)
        {
            searchSubgraph.unset(currlineInd);
            currMaxClique.set(currlineInd);
            ++currCliqueSize;

            bitset_type line = this->globalAdjMatr[currLine.first];
            line &= searchSubgraph;

            if (countSetBits(line) > 0)
            {
                std::vector<std::pair<size_t, size_t>> coloredVerts = this->retColoring(line.getNeighbours(), maxCliqueSize - currCliqueSize + 1);
                this->maxCliqueSegTest(line, coloredVerts, currMaxClique);
            }
            else
            {
                if (currCliqueSize > maxCliqueSize)
                {
                    this->globalMaxClique = currMaxClique;
                }
            }
            currMaxClique.unset(currlineInd);
            --currCliqueSize;
        }
    }
}

void SegundoAlgorithm::maxCliqueFindingSegundoReference(bitset_type searchSubgraph,
    std::vector<std::pair<size_t, size_t>>& allowedVerts, bitset_type& currMaxClique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = allowedVerts.back();
        size_t currLineInd = currLine.first;

        allowedVerts.pop_back();
        searchSubgraph.unset(currLineInd);

        int32_t currCliqueSize = countSetBits(currMaxClique);
        int32_t maxCliqueSize = countSetBits(this->globalMaxClique);
        
        if (currCliqueSize + currLine.second > maxCliqueSize)
        {
            currMaxClique.set(currLineInd);
            ++currCliqueSize;

            bitset_type line = this->globalAdjMatr[currLine.first];
            line &= searchSubgraph;

            if (countSetBits(line) != 0)
            {
                std::vector<std::pair<size_t, size_t>> coloredVerts = this->coloring(line.getNeighbours(),
                    static_cast<int32_t>(maxCliqueSize - currCliqueSize + 1), line);
                maxCliqueFindingSegundoReference(line, coloredVerts, currMaxClique);
            }
            else
            {
                if (currCliqueSize > maxCliqueSize)
                {
                    this->globalMaxClique = currMaxClique;
                }
            }
            currMaxClique.unset(currLineInd);
            --currCliqueSize;
        }
        else break;
    }
}

// void SegundoAlgorithm::maxCliqueFindingSegundoUsingAdditionalMatrix(std::map<size_t, bitset_type>& adjMatr,
//         index_lines_with_order& allowedVerts, std::set<size_t>& currMaxCLique)
// {
//     ++step_count;
//     while (!allowedVerts.empty())
//     {
//         auto currLine = allowedVerts.back();
//         // auto currLine = *(--allowedVerts.end());
//         size_t currLineInd = currLine.getId();
        
//         // allowedVerts.erase((--allowedVerts.end()));
//         // adjMatr.erase(currLineInd);
//         allowedVerts.pop_back();
        
//         if (currMaxCLique.size() + currLine.getColor() > this->maxClique.size())
//         {
//             currMaxCLique.insert(currLineInd);
//             // Get all near verticies that was conjuncted with currLine
//             adjMatr.erase(currLineInd);
//             index_lines nearVerts = this->getNeighbours(adjMatr, currLine);

//             if (!nearVerts.empty())
//             {
//                 index_lines_with_order coloredVerts = this->coloringUsingAdditionalMatrix(nearVerts, static_cast<int32_t>(this->maxClique.size() - currMaxCLique.size() + 1));
//                 maxCliqueFindingSegundoUsingAdditionalMatrix(nearVerts, coloredVerts, currMaxCLique);
//             }
//             else
//             {
//                 if (currMaxCLique.size() > this->maxClique.size())
//                 {
//                     this->maxClique = currMaxCLique;
//                 }
//             }
//             currMaxCLique.erase(currLineInd);
//         }
//     }
// }

// SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighbours(index_lines& adjMatr, bitset_type& currBitset)
// {
//     index_lines retVerticies{};
//     size_t dimSize = currBitset.getDimSize();
//     currBitset.unset(currBitset.getId());
//     bitset_type localBitset = currBitset;
//     size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
//     while (firstSetBitPos < dimSize)
//     {
//         if (adjMatr.contains(firstSetBitPos))
//         {
//             bitset_type line = adjMatr[firstSetBitPos];
//             line &= currBitset;
//             retVerticies.insert({ firstSetBitPos, std::move(line) });
//         }
//         localBitset.unset(firstSetBitPos);
//         firstSetBitPos = localBitset.getFirstNonZeroPosition();
//     }
//     return retVerticies;
// }

// SegundoAlgorithm::index_lines SegundoAlgorithm::getNeighboursBitsetInput(bitset_type& currBitset)
// {
//     index_lines retVerticies{};
//     size_t dimSize = currBitset.getDimSize();
//     bitset_type localBitset = currBitset;
//     size_t firstSetBitPos = localBitset.getFirstNonZeroPosition();
//     while (firstSetBitPos < dimSize)
//     {
//         if (this->globalAdjMatr.contains(firstSetBitPos))
//         {
//             bitset_type line = this->globalAdjMatr[firstSetBitPos];
//             line &= currBitset;
//             retVerticies.insert({ firstSetBitPos, std::move(line) });
//         }
//         localBitset.unset(firstSetBitPos);
//         firstSetBitPos = localBitset.getFirstNonZeroPosition();
//     }
//     return retVerticies;
// }
