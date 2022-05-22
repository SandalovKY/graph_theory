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
    void runMaxCliqueFinding(std::map<size_t, bitset_type>& adjMatr, bool useModAlgorithm = false);
private:
    index_lines coloring(index_lines& adjMatr, int32_t minCol);    //
    index_lines coloring_mod_1(index_lines& adjMatr, int32_t minCol);    //
    index_lines getNeighbours(index_lines& adjMatr, bitset_type& currBitset);   //
    index_lines getNeighbours_mod_1(index_lines& adjMatr, bitset_type& currBitset);    //
    void maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique);
    void maxCliqueFindingSegundo_mod_1(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique);
};