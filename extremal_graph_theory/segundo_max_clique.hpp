#pragma once

#include <set>
#include <map>
#include "bitset.hpp"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

enum class Algorithms
{
    // default algorithm value
    Empty,
    // Segundo algorithm modifications with implemented bitset type
    SimpleHeuristic,
    Heuristic,
    Reference,
    Modified,
    BoostedReference,
    BoostedModified,
    BronKerboschAlgorithm,
    // Segundo algorithm modifications with boost bitset type
    ReferenceUsingBoost,
    HeuristicUsingBoost
};

template<typename bitsetType>
class SegundoAlgorithm
{
public:
    using bitset_type = bitsetType;
    using indexed_lines = std::map<size_t, bitset_type>;
    using colored_verts = std::vector<std::pair<size_t, size_t> >;

    bitset_type m_globalMaxClique{};
    indexed_lines m_globalAdjMatr{};
    bool m_use_simple_heuristic{ false };
    size_t m_step_count{ 0 };

    size_t m_maximalKerboshClique{};

    SegundoAlgorithm(indexed_lines adjMatr);
    void runAlgorithm(Algorithms alg){}
private:
    void maxCliqueFindingSegundoModified(bitset_type searchSubgraph, colored_verts& allowedVerts, bitset_type& currMaxClique);
    void maxCliqueFindingSegundoReference(bitset_type searchSubgraph, colored_verts& allowedVerts, bitset_type& currMaxClique);
    void maxCliqueFindingSegundoReferenceBoost(bitset_type searchSubgraph, colored_verts& allowedVerts, bitset_type& currMaxClique);
    size_t bronKerbosch(bitset_type& currClique, bitset_type& remainingNodes, bitset_type& skipNodes, size_t depth = 0){ return 0; }

    colored_verts coloringReferenceBoost(bitset_type currVerts, int32_t minCol);
    colored_verts coloringReference(bitset_type currVerts, int32_t minCol);
    colored_verts coloringModified(bitset_type currVerts, int32_t minCol);
};

template<>
SegundoAlgorithm<myBitset<> >::colored_verts SegundoAlgorithm<myBitset<> >::coloringReference(bitset_type currVerts, int32_t minCol);
template<>
SegundoAlgorithm<boost::dynamic_bitset<> >::colored_verts
SegundoAlgorithm<boost::dynamic_bitset<> >::coloringReferenceBoost(boost::dynamic_bitset<> currVerts, int32_t minCol);
template<>
SegundoAlgorithm<myBitset<> >::colored_verts SegundoAlgorithm<myBitset<> >::coloringModified(bitset_type notColoredVerts, int32_t minCol);

template<>
void SegundoAlgorithm<myBitset<> >::maxCliqueFindingSegundoModified(bitset_type searchSubgraph,
    std::vector<std::pair<size_t, size_t>>& allowedVerts, bitset_type& currMaxClique);
template<>
void SegundoAlgorithm<myBitset<> >::maxCliqueFindingSegundoReference(bitset_type searchSubgraph,
    std::vector<std::pair<size_t, size_t>>& allowedVerts, bitset_type& currMaxClique);
template<>
void SegundoAlgorithm<boost::dynamic_bitset<> >::maxCliqueFindingSegundoReferenceBoost(bitset_type searchSubgraph,
    colored_verts& allowedVerts, bitset_type& currMaxClique);
template<>
size_t SegundoAlgorithm<myBitset<> >::bronKerbosch(bitset_type& currClique,
    bitset_type& remainingNodes, bitset_type& skipNodes, size_t depth);
