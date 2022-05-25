#pragma once

#include <set>
#include <map>
#include "bitset.h"

class SegundoAlgorithm
{
public:
    using bitset_type = myDynamicBitset<>;
    using index_lines = std::map<size_t, bitset_type>;
    std::set<size_t> maxClique{};

    bitset_type globalMaxClique{};
    index_lines globalAdjMatr{};

    SegundoAlgorithm(const index_lines& adjMatr);

    void runMaxCliqueFinding(std::map<size_t, bitset_type> adjMatr, bool useModAlgorithm = false);
    void runTestAlgorithm();
private:
    index_lines coloring(index_lines& adjMatr, int32_t minCol);
    index_lines getNeighbours(index_lines& adjMatr, bitset_type& currBitset);
    void maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique);

    index_lines coloringUsingAdditionalMatrix(index_lines& adjMatr, int32_t minCol);
    index_lines getNeighboursUsingAdditionalMatrix(index_lines& adjMatr, bitset_type& currBitset);
    void maxCliqueFindingSegundoUsingAdditionalMatrix(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique);
    index_lines getNeighboursTest(bitset_type& currBitset);
    void maxCliqueSegTest(bitset_type searchSubgraph, index_lines& allowedVerts, bitset_type& currMaxClique);
};