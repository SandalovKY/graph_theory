#pragma once

#include <set>
#include <map>
#include "bitset.hpp"

class SegundoAlgorithm
{
public:
    using bitset_type = myDynamicBitset<>;
    using indexed_lines = std::map<size_t, bitset_type>;
    using colored_verts = std::vector<std::pair<size_t, size_t> >;

    enum Algorithms
    {
        Reference,
        ReferenceWithInputBitset,
        Modified,
        ModifiedWithInputBitset,
        BoostedReferenceAlgorithm,
        BoostedModifiedAlgorithm
    };

    bitset_type globalMaxClique{};
    indexed_lines globalAdjMatr{};

    SegundoAlgorithm(indexed_lines adjMatr);
    void runTestAlgorithm(Algorithms alg);
private:
    void maxCliqueFindingSegundoModified(bitset_type searchSubgraph, colored_verts& allowedVerts, bitset_type& currMaxClique);
    void maxCliqueFindingSegundoReference(bitset_type searchSubgraph,
    colored_verts& allowedVerts, bitset_type& currMaxClique);

    colored_verts coloring(const std::set<size_t>& adjMatr, int32_t minCol, const bitset_type& currVerts);
    colored_verts coloringModified(const std::set<size_t>& currVerts, int32_t minCol);
};
