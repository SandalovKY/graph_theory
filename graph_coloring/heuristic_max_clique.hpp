#pragma once

#include <map>
#include <vector>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "bitset.hpp"
#include "segundo_max_clique.hpp"

SegundoAlgorithm::bitset_type maxCliqueFindingHeuristic(std::map<size_t, myBitset<>> adjMatr);
SegundoAlgorithm::bitset_type maxCliqueFindingHeuristicSimple(std::map<size_t, myBitset<>> adjMatr);
boost::dynamic_bitset<> maxCliqueFindingHeuristicSimpleBoost(std::map<size_t, boost::dynamic_bitset<>> adjMatr);
