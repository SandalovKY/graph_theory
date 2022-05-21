#pragma once

#include <set>
#include <map>
#include "bitset.h"

class SegundoAlgorithm
{
public:
    using bitset_type = myDynamicBitset<>;
    using index_lines = std::map<size_t, bitset_type>;
    using index_lines_with_flags = std::map<size_t, std::pair<bitset_type, bool>>;
    std::set<size_t> maxClique{};
    void runMaxCliqueFinding(std::map<size_t, bitset_type>& adjMatr);
private:
    index_lines coloring(index_lines& adjMatr, int32_t currMaxCliqueSize);    //
    index_lines getNeighbours(const index_lines& adjMatr, bitset_type& currBitset);   //
    void maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr,
        std::map<size_t, bitset_type>& allowedVerts, std::set<size_t>& currMaxCLique);
};