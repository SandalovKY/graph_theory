#pragma once

#include <set>
#include <map>
#include "bitset.hpp"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

class SegundoAlgorithm
{
public:
    using bitset_type = myBitset<>;
    using indexed_lines = std::map<size_t, bitset_type>;
    using colored_verts = std::vector<std::pair<size_t, size_t> >;

    enum Algorithms
    {
        SimpleHeuristic,
        Heuristic,
        Reference,
        ReferenceUsingBoost,
        Modified,
        BoostedReference,
        BoostedModified,
        BronKerboschAlgorithm,
        HeuristicUsingBoost
    };

    bitset_type m_globalMaxClique{};
    indexed_lines m_globalAdjMatr{};
    bool m_use_simple_heuristic{ false };
    size_t m_step_count{ 0 };

    // boost::dynamic_bitset<> m_boostBitset{};
    // std::map<size_t, boost::dynamic_bitset<> > m_globalAdjMatrBoost{};

    size_t m_maximalKerboshClique{};

    SegundoAlgorithm(indexed_lines adjMatr);
    void runTestAlgorithm(Algorithms alg);
private:
    void maxCliqueFindingSegundoModified(bitset_type searchSubgraph, colored_verts& allowedVerts, bitset_type& currMaxClique);
    void maxCliqueFindingSegundoReference(bitset_type searchSubgraph, colored_verts& allowedVerts, bitset_type& currMaxClique);
    // void maxCliqueFindingSegundoReferenceBoost(boost::dynamic_bitset<> searchSubgraph, colored_verts& allowedVerts, boost::dynamic_bitset<>& currMaxClique);
    size_t bronKerbosch(bitset_type& currClique, bitset_type& remainingNodes, bitset_type& skipNodes, size_t depth = 0);

    colored_verts coloringReference(bitset_type currVerts, int32_t minCol);
    // std::vector<std::pair<size_t, size_t>> coloringReferenceBoost(boost::dynamic_bitset<> currVerts, int32_t minCol);
    colored_verts coloringModified(bitset_type currVerts, int32_t minCol);
};
