#pragma once

#include <map>
#include <vector>
#include "bitset.hpp"
#include "segundo_max_clique.hpp"

SegundoAlgorithm::bitset_type maxCliqueFindingHeuristic(std::map<size_t, myDynamicBitset<>> adjMatr);
SegundoAlgorithm::bitset_type maxCliqueFindingHeuristicSimple(std::map<size_t, myDynamicBitset<>> adjMatr);
