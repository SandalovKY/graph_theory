#pragma once

#include <set>
#include <map>
#include "bitset.hpp"

class SegundoAlgorithm
{
public:
    using bitset_type = myDynamicBitset<>;
    using index_lines = std::map<size_t, bitset_type>;
    using index_lines_with_order = std::vector<bitset_type>;
    enum Algorithms
    {
        Reference,
        ReferenceWithInputBitset,
        Modified,
        ModifiedWithInputBitset
    };
    std::set<size_t> maxClique{};

    bitset_type globalMaxClique{};
    index_lines globalAdjMatr{};

    SegundoAlgorithm(index_lines adjMatr);

    void runTestAlgorithm(Algorithms alg);
private:
    index_lines_with_order coloring(index_lines& adjMatr, int32_t minCol);
    index_lines_with_order coloringUsingAdditionalMatrix(index_lines& adjMatr, int32_t minCol);
    index_lines getNeighbours(index_lines& adjMatr, bitset_type& currBitset);
    index_lines getNeighboursBitsetInput(bitset_type& currBitset);

    void maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr,
        index_lines_with_order& allowedVerts, std::set<size_t>& currMaxCLique);
    void maxCliqueFindingSegundoUsingAdditionalMatrix(std::map<size_t, bitset_type>& adjMatr,
        index_lines_with_order& allowedVerts, std::set<size_t>& currMaxCLique);
    void maxCliqueSegTest(bitset_type searchSubgraph, index_lines_with_order& allowedVerts, bitset_type& currMaxClique);
    void maxCliqueFindingSegundoReference(bitset_type searchSubgraph,
    index_lines_with_order& allowedVerts, std::set<size_t>& currMaxCLique);
};