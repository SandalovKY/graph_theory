#include <set>
#include <map>
#include "bitset.h"

class SegundoAlgorithm
{
public:
    using bitset_type = myDynamicBitset<>;
    std::set<size_t> maxClique{};
    void runMaxCliqueFinding(std::map<size_t, bitset_type>& adjMatr);
private:
    std::map<size_t, bitset_type> coloring(std::map<size_t, std::pair<bitset_type, bool>> adjMatr);
    std::map<size_t, std::pair<bitset_type, bool>> getNeighbours(std::map<size_t, bitset_type>& adjMatr, bitset_type& currBitset);
    void maxCliqueFindingSegundo(std::map<size_t, bitset_type>& adjMatr, std::set<size_t>& currMaxCLique);
};