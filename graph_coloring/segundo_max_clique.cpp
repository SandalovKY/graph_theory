#include "segundo_max_clique.hpp"
#include "heuristic_max_clique.hpp"

#include <iostream>

int32_t step_count{ 0 };

SegundoAlgorithm::SegundoAlgorithm(indexed_lines adjMatr)
    : m_globalAdjMatr(std::move(adjMatr))
{
    if (!m_globalAdjMatr.empty())
    {
        this->m_globalMaxClique = bitset_type(m_globalAdjMatr.begin()->second.getDimSize());
        // this->m_boostBitset = boost::dynamic_bitset<>(m_globalAdjMatr.begin()->second.getDimSize());
    }
}

void SegundoAlgorithm::runTestAlgorithm(Algorithms algorithm)
{
    if (this->m_globalAdjMatr.empty()) return;

    this->m_globalMaxClique.all2zero();
    step_count = 0;
    size_t numBits{ this->m_globalMaxClique.getDimSize() };
    std::vector<std::pair<size_t, size_t>> coloredVec{};

    bitset_type inputVerts(numBits);
    bitset_type currMaxClique(numBits);
    inputVerts.all2one();

    // boost::dynamic_bitset<> inputVertsBoost(numBits);
    // boost::dynamic_bitset<> currMaxCliqueBoost(numBits);
    // inputVertsBoost.set();

    bitset_type brKerbCurrClique(numBits);
    bitset_type brKerbtabu(numBits);

    switch (algorithm)
    {
    case Algorithms::Heuristic:
        this->m_globalMaxClique = maxCliqueFindingHeuristic(this->m_globalAdjMatr);
        break;
    case Algorithms::SimpleHeuristic:
        this->m_globalMaxClique = maxCliqueFindingHeuristicSimple(this->m_globalAdjMatr);
        break;
    // case Algorithms::HeuristicUsingBoost:
    //     this->m_boostBitset = maxCliqueFindingHeuristicSimpleBoost(this->m_globalAdjMatrBoost);
    //     break;
    case Algorithms::BoostedReference:
        this->m_globalMaxClique = maxCliqueFindingHeuristic(this->m_globalAdjMatr);
    case Algorithms::Reference:
        coloredVec = this->coloringReference(inputVerts, 3);
        this->maxCliqueFindingSegundoReference(inputVerts, coloredVec, currMaxClique);
        break;
    // case Algorithms::ReferenceUsingBoost:
    //     coloredVec = this->coloringReferenceBoost(inputVertsBoost, 3);
    //     this->maxCliqueFindingSegundoReferenceBoost(inputVertsBoost, coloredVec, currMaxCliqueBoost);
    //     break;
    case Algorithms::BoostedModified:
        this->m_globalMaxClique = maxCliqueFindingHeuristic(this->m_globalAdjMatr);
    case Algorithms::Modified:
        coloredVec = this->coloringModified(inputVerts, 3);
        this->maxCliqueFindingSegundoModified(inputVerts, coloredVec, currMaxClique);
        break;
    case Algorithms::BronKerboschAlgorithm:
        this->bronKerbosch(brKerbCurrClique, inputVerts, brKerbtabu);
        break;
    default:
        break;
    }
    std::cout << "Num steps: " << step_count << std::endl;
}

std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::coloringReference(bitset_type currVerts, int32_t minCol)
{
    std::vector<std::pair<size_t, size_t>> retColored{};
    if (this->m_globalAdjMatr.empty())
    {
        return retColored;
    }
    size_t numBits{ this->m_globalAdjMatr.begin()->second.getDimSize() };

    int32_t currColor{ 1 };

    bitset_type localVerts(currVerts);
    size_t nextVert = localVerts.getFirstNonZeroPosition();
    bitset_type coloredVerts(numBits);
    coloredVerts.all2one();

    while (nextVert < numBits)
    {
        while (nextVert < numBits)
        {
            localVerts.unset(nextVert);
            coloredVerts.unset(nextVert);
            if (currColor >= minCol)
            {
                retColored.push_back({ nextVert, currColor });
            }
            localVerts &= ~(this->m_globalAdjMatr[nextVert]);
            ~(this->m_globalAdjMatr[nextVert]);
            nextVert = localVerts.getFirstNonZeroPosition();
        }
        currVerts &= coloredVerts;
        localVerts = currVerts;
        nextVert = localVerts.getFirstNonZeroPosition();
        ++currColor;
    }
    return retColored;
}

// std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::coloringReferenceBoost(boost::dynamic_bitset<> currVerts, int32_t minCol)
// {
//     std::vector<std::pair<size_t, size_t>> retColored{};
//     if (this->m_globalAdjMatrBoost.empty())
//     {
//         return retColored;
//     }
//     size_t numBits{ this->m_globalAdjMatrBoost.begin()->second.size() };

//     int32_t currColor{ 1 };

//     boost::dynamic_bitset<> localVerts(currVerts);
//     size_t nextVert = localVerts.find_first();
//     boost::dynamic_bitset<> coloredVerts(numBits);
//     coloredVerts.set();

//     while (nextVert < numBits)
//     {
//         while (nextVert < numBits)
//         {
//             localVerts.set(nextVert, false);
//             coloredVerts.set(nextVert, false);
//             if (currColor >= minCol)
//             {
//                 retColored.push_back({ nextVert, currColor });
//             }
//             localVerts &= ~(this->m_globalAdjMatrBoost[nextVert]);
//             nextVert = localVerts.find_first();
//         }
//         currVerts &= coloredVerts;
//         localVerts = currVerts;
//         nextVert = localVerts.find_first();
//         ++currColor;
//     }
//     return retColored;
// }

std::vector<std::pair<size_t, size_t>> SegundoAlgorithm::coloringModified(bitset_type notColoredVerts, int32_t minCol)
{
    std::vector<std::pair<size_t, size_t>> retColored{};
    std::map<size_t, std::set<size_t>> coloredVerts{};
    if (this->m_globalAdjMatr.empty())
    {
        return retColored;
    }
    size_t numBits{ this->m_globalAdjMatr.begin()->second.getDimSize() };
    size_t numVerts = countSetBits(notColoredVerts);

    std::vector<bitset_type> tabuCols(numVerts, bitset_type(numBits));

    size_t nextVert = notColoredVerts.getFirstNonZeroPosition();
    while (nextVert < numBits)
    {
        int32_t allowedColor { 0 };
        while (tabuCols[allowedColor].isBitSet(nextVert))
        {
            ++allowedColor;
        }
        tabuCols[allowedColor] |= this->m_globalAdjMatr[nextVert];
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
        searchSubgraph.unset(currlineInd);

        allowedVerts.pop_back();

        int32_t currCliqueSize = countSetBits(currMaxClique);
        int32_t maxCliqueSize = countSetBits(this->m_globalMaxClique);

        if (currCliqueSize + currLine.second > maxCliqueSize)
        {
            currMaxClique.set(currlineInd);
            ++currCliqueSize;

            bitset_type line = this->m_globalAdjMatr[currLine.first];
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
                    this->m_globalMaxClique = currMaxClique;
                }
            }
            currMaxClique.unset(currlineInd);
            --currCliqueSize;
        }
        else break;
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
        int32_t maxCliqueSize = countSetBits(this->m_globalMaxClique);
        
        if (currCliqueSize + currLine.second > maxCliqueSize)
        {
            currMaxClique.set(currLineInd);
            ++currCliqueSize;

            bitset_type line = this->m_globalAdjMatr[currLine.first];
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
                    this->m_globalMaxClique = currMaxClique;
                }
            }
            currMaxClique.unset(currLineInd);
            --currCliqueSize;
        }
        else break;
    }
}

// void SegundoAlgorithm::maxCliqueFindingSegundoReferenceBoost(boost::dynamic_bitset<> searchSubgraph, colored_verts& allowedVerts, boost::dynamic_bitset<>& currMaxClique)
// {
//     ++step_count;
//     while (!allowedVerts.empty())
//     {
//         auto currLine = allowedVerts.back();
//         size_t currLineInd = currLine.first;

//         allowedVerts.pop_back();
//         searchSubgraph.set(currLineInd, false);

//         int32_t currCliqueSize = currMaxClique.count();
//         int32_t maxCliqueSize = this->m_boostBitset.count();
        
//         if (currCliqueSize + currLine.second > maxCliqueSize)
//         {
//             currMaxClique.set(currLineInd);
//             ++currCliqueSize;

//             boost::dynamic_bitset<> line = this->m_globalAdjMatrBoost[currLine.first];
//             line &= searchSubgraph;

//             if (line.count() != 0)
//             {
//                 std::vector<std::pair<size_t, size_t>> coloredVerts = this->coloringReferenceBoost(line,
//                     static_cast<int32_t>(maxCliqueSize - currCliqueSize + 1));
//                 maxCliqueFindingSegundoReferenceBoost(line, coloredVerts, currMaxClique);
//             }
//             else
//             {
//                 if (currCliqueSize > maxCliqueSize)
//                 {
//                     this->m_boostBitset = currMaxClique;
//                 }
//             }
//             currMaxClique.set(currLineInd, false);
//             --currCliqueSize;
//         }
//         else break;
//     }
// }

size_t SegundoAlgorithm::bronKerbosch(bitset_type& currClique, bitset_type& remainingNodes, bitset_type& skipNodes, size_t depth)
{
    if (this->m_globalAdjMatr.empty()) return 0;
    size_t numBits = this->m_globalAdjMatr.begin()->second.getDimSize();
    if (remainingNodes.getFirstNonZeroPosition() >= numBits && skipNodes.getFirstNonZeroPosition() >= numBits)
    {
        size_t currCliqueSize = countSetBits(currClique);
        this->m_maximalKerboshClique = std::max(currCliqueSize, this->m_maximalKerboshClique);
        return 1;
    }

    size_t foundCliques{ 0 };

    SegundoAlgorithm::bitset_type pivotSet(remainingNodes | skipNodes);

    size_t pivotVert = pivotSet.getFirstNonZeroPosition();
    SegundoAlgorithm::bitset_type uNbhd(this->m_globalAdjMatr[pivotVert]);
    ~uNbhd;
    uNbhd &= remainingNodes;

    size_t nextVert = uNbhd.getFirstNonZeroPosition();
    while (nextVert < numBits)
    {
        bitset_type newCurrClique = currClique;
        newCurrClique.set(nextVert);
        bitset_type newRemainingNodes = remainingNodes & this->m_globalAdjMatr[nextVert];
        bitset_type newSkipNodes = skipNodes & this->m_globalAdjMatr[nextVert];
        foundCliques += bronKerbosch(newCurrClique, newRemainingNodes, newSkipNodes, depth + 1);

        remainingNodes.unset(nextVert);
        skipNodes.set(nextVert);
        uNbhd.unset(nextVert);
        nextVert = uNbhd.getFirstNonZeroPosition();
    }
    return foundCliques;
}
