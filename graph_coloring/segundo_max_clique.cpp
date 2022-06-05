#include "segundo_max_clique.hpp"
#include "heuristic_max_clique.hpp"

#include <iostream>

int32_t step_count{ 0 };

SegundoAlgorithm::SegundoAlgorithm(indexed_lines adjMatr)
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
    case Algorithms::Heuristic:
        this->globalMaxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
        break;
    case Algorithms::BoostedReferenceAlgorithm:
        this->globalMaxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
    case Algorithms::Reference:
        coloredVec = this->coloringReference(inputVerts, 3);
        this->maxCliqueFindingSegundoReference(inputVerts, coloredVec, currMaxClique);
        break;
    case Algorithms::BoostedModifiedAlgorithm:
        this->globalMaxClique = maxCliqueFindingHeuristic(this->globalAdjMatr);
    case Algorithms::Modified:
        coloredVec = this->coloringModified(inputVerts, 3);
        this->maxCliqueFindingSegundoModified(inputVerts, coloredVec, currMaxClique);
        break;
    default:
        break;
    }
    std::cout << "Num steps: " << step_count << std::endl;
}

std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::coloringReference(bitset_type currVerts, int32_t minCol)
{
    std::vector<std::pair<size_t, size_t>> retColored{};
    if (this->globalAdjMatr.empty())
    {
        return retColored;
    }
    size_t numBits{ this->globalAdjMatr.begin()->second.getDimSize() };

    int32_t currColor{ 1 };

    bitset_type localVerts(currVerts);
    size_t nextVert = localVerts.getFirstNonZeroPosition();

    while (nextVert < numBits)
    {
        bitset_type coloredVerts(numBits, -1);
        std::set<size_t> vertsWithCurrColor{};
        while (nextVert < numBits)
        {
            localVerts.unset(nextVert);
            coloredVerts.set(nextVert);
            vertsWithCurrColor.insert(nextVert);
            bitset_type vertLine = this->globalAdjMatr[nextVert];
            localVerts = localVerts & ~(vertLine);
            nextVert = localVerts.getFirstNonZeroPosition();
        }
        currVerts &= ~coloredVerts;
        localVerts = currVerts;
        if (currColor >= minCol)
        {
            for (const auto& vert: vertsWithCurrColor)
            {
                retColored.push_back({ vert, currColor });
            }
        }
        nextVert = localVerts.getFirstNonZeroPosition();
        ++currColor;
    }
    return retColored;
}

std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::coloringModified(bitset_type notColoredVerts, int32_t minCol)
{
    std::vector<std::pair<size_t, size_t>> retColored{};
    std::map<size_t, std::set<size_t>> coloredVerts{};
    if (this->globalAdjMatr.empty())
    {
        return retColored;
    }
    size_t numBits{ this->globalAdjMatr.begin()->second.getDimSize() };
    size_t numVerts = countSetBits(notColoredVerts);

    std::vector<bitset_type> tabuCols(numVerts, bitset_type(numBits, -1));

    size_t nextVert = notColoredVerts.getFirstNonZeroPosition();
    while (nextVert < numBits)
    {
        int32_t allowedColor { 0 };
        while (tabuCols[allowedColor].isBitSet(nextVert))
        {
            ++allowedColor;
        }
        tabuCols[allowedColor] |= this->globalAdjMatr[nextVert];
        notColoredVerts.unset(nextVert);
        if (allowedColor + 1 >= minCol)
        {
            coloredVerts[allowedColor + 1].insert(nextVert);
        }
        nextVert = notColoredVerts.getFirstNonZeroPosition();
    }
    for (const auto& verts: coloredVerts)
    {
        for (const auto& vert: verts.second)
        {
            retColored.push_back({ vert, verts.first });
        }
    }
    return retColored;
}

void SegundoAlgorithm::maxCliqueFindingSegundoModified(bitset_type searchSubgraph, std::vector<std::pair<size_t, size_t>>& allowedVerts, bitset_type& currMaxClique)
{
    ++step_count;
    while (!allowedVerts.empty())
    {
        auto currLine = allowedVerts.back();
        size_t currlineInd = currLine.first;
        // searchSubgraph.unset(currlineInd);

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
                std::vector<std::pair<size_t, size_t>> coloredVerts = this->coloringModified(line, maxCliqueSize - currCliqueSize + 1);
                this->maxCliqueFindingSegundoModified(line, coloredVerts, currMaxClique);
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
        // else break;
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
                std::vector<std::pair<size_t, size_t>> coloredVerts = this->coloringReference(line,
                    static_cast<int32_t>(maxCliqueSize - currCliqueSize + 1));
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
